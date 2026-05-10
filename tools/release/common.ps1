Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-ReleaseRepoRoot {
    return (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

function Get-ReleaseVersionsPath {
    return Join-Path (Get-ReleaseRepoRoot) "tools/release/versions.toml"
}

function Get-ReleaseScratchRoot {
    $scratchRoot = Join-Path (Get-ReleaseRepoRoot) ".git/release-check"
    New-Item -ItemType Directory -Path $scratchRoot -Force | Out-Null
    return $scratchRoot
}

function Assert-ToolAvailable {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ToolName
    )

    if (-not (Get-Command $ToolName -ErrorAction SilentlyContinue)) {
        throw "Required tool '$ToolName' is not available in PATH."
    }
}

function Get-ReleaseVersions {
    $versionsPath = Get-ReleaseVersionsPath
    if (-not (Test-Path -LiteralPath $versionsPath)) {
        throw "Release version file not found: $versionsPath"
    }

    $versions = @{}
    $section = $null

    foreach ($rawLine in Get-Content -LiteralPath $versionsPath) {
        $line = ($rawLine -replace '\s+#.*$', '').Trim()
        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }

        if ($line -match '^\[(?<section>[A-Za-z0-9_-]+)\]$') {
            $section = $Matches.section
            if (-not $versions.ContainsKey($section)) {
                $versions[$section] = @{}
            }
            continue
        }

        if ($line -match '^version\s*=\s*"(?<version>\d+\.\d+\.\d+)"$') {
            if (-not $section) {
                throw "Found version entry outside a section in $versionsPath"
            }

            $versions[$section]["version"] = $Matches.version
        }
    }

    return $versions
}

function Get-ReleaseVersion {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("desktop", "mcp")]
        [string]$Component
    )

    $versions = Get-ReleaseVersions
    if (-not $versions.ContainsKey($Component) -or -not $versions[$Component].ContainsKey("version")) {
        throw "Missing version for [$Component] in $(Get-ReleaseVersionsPath)"
    }

    return $versions[$Component]["version"]
}

function Get-ReleaseTag {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("desktop", "mcp")]
        [string]$Component
    )

    $version = Get-ReleaseVersion -Component $Component
    switch ($Component) {
        "desktop" { return "desktop-v$version" }
        "mcp" { return "mcp-v$version" }
        default { throw "Unsupported release component: $Component" }
    }
}

function Invoke-CheckedCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [string[]]$Arguments = @(),

        [string]$WorkingDirectory = (Get-ReleaseRepoRoot)
    )

    Push-Location $WorkingDirectory
    try {
        & $FilePath @Arguments
        if ($LASTEXITCODE -ne 0) {
            $display = @($FilePath) + $Arguments -join ' '
            throw "Command failed with exit code ${LASTEXITCODE}: $display"
        }
    }
    finally {
        Pop-Location
    }
}

function Get-CheckedCommandOutput {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [string[]]$Arguments = @(),

        [string]$WorkingDirectory = (Get-ReleaseRepoRoot)
    )

    Push-Location $WorkingDirectory
    try {
        $output = & $FilePath @Arguments 2>&1
        $exitCode = $LASTEXITCODE
        if ($exitCode -ne 0) {
            $display = @($FilePath) + $Arguments -join ' '
            throw "Command failed with exit code ${exitCode}: $display`n$($output -join [Environment]::NewLine)"
        }

        return ($output -join [Environment]::NewLine).Trim()
    }
    finally {
        Pop-Location
    }
}

function Assert-GitWorktreeClean {
    $status = Get-CheckedCommandOutput -FilePath "git" -Arguments @("status", "--porcelain")
    if (-not [string]::IsNullOrWhiteSpace($status)) {
        throw "Git worktree is not clean. Commit or stash changes before creating a release tag."
    }
}

function Assert-NotDetachedHead {
    $branchName = Get-CheckedCommandOutput -FilePath "git" -Arguments @("rev-parse", "--abbrev-ref", "HEAD")
    if ($branchName -eq "HEAD") {
        throw "Release tags must be created from a branch, not from detached HEAD."
    }
}

function Fetch-RemoteTags {
    param(
        [string]$Remote = "origin"
    )

    Invoke-CheckedCommand -FilePath "git" -Arguments @("fetch", $Remote, "--tags", "--force")
}

function Assert-TagMissing {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Tag,

        [string]$Remote = "origin"
    )

    $localTag = Get-CheckedCommandOutput -FilePath "git" -Arguments @("tag", "--list", $Tag)
    if (-not [string]::IsNullOrWhiteSpace($localTag)) {
        throw "Git tag '$Tag' already exists locally."
    }

    $remoteTag = Get-CheckedCommandOutput -FilePath "git" -Arguments @("ls-remote", "--tags", $Remote, "refs/tags/$Tag")
    if (-not [string]::IsNullOrWhiteSpace($remoteTag)) {
        throw "Git tag '$Tag' already exists on remote '$Remote'."
    }
}