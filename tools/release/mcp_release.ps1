param(
    [string]$Remote = "origin",
    [switch]$SkipBuildCheck,
    [switch]$SkipPush
)

. (Join-Path $PSScriptRoot "common.ps1")

Assert-ToolAvailable -ToolName "git"
Assert-ToolAvailable -ToolName "python"

$repoRoot = Get-ReleaseRepoRoot
$version = Get-ReleaseVersion -Component "mcp"
$tag = Get-ReleaseTag -Component "mcp"
$mcpProjectDir = Join-Path $repoRoot "LEO_sniffy/tools/sniffy_mcp"

Push-Location $repoRoot
try {
    Assert-GitWorktreeClean
    Assert-NotDetachedHead
    Fetch-RemoteTags -Remote $Remote
    Assert-TagMissing -Tag $tag -Remote $Remote

    Invoke-CheckedCommand -FilePath "python" -Arguments @("LEO_sniffy/tools/sniffy_mcp/release_metadata.py", "validate", "--expected-tag", $tag) -WorkingDirectory $repoRoot

    if (-not $SkipBuildCheck) {
        $scratchRoot = Get-ReleaseScratchRoot
        $scratchProjectDir = Join-Path $scratchRoot "mcp-src"
        $scratchVersionDir = Join-Path $scratchRoot "tools/release"
        $venvDir = Join-Path $scratchRoot "mcp-venv"
        $venvPython = Join-Path $venvDir "Scripts/python.exe"

        if (Test-Path -LiteralPath $scratchProjectDir) {
            Remove-Item -LiteralPath $scratchProjectDir -Recurse -Force
        }
        if (Test-Path -LiteralPath (Join-Path $scratchRoot "tools")) {
            Remove-Item -LiteralPath (Join-Path $scratchRoot "tools") -Recurse -Force
        }

        New-Item -ItemType Directory -Path $scratchProjectDir -Force | Out-Null
        Get-ChildItem -LiteralPath $mcpProjectDir -Force | Copy-Item -Destination $scratchProjectDir -Recurse -Force

        New-Item -ItemType Directory -Path $scratchVersionDir -Force | Out-Null
        Copy-Item -LiteralPath (Join-Path $repoRoot "tools/release/versions.toml") -Destination (Join-Path $scratchVersionDir "versions.toml") -Force

        if (-not (Test-Path -LiteralPath $venvPython)) {
            Invoke-CheckedCommand -FilePath "python" -Arguments @("-m", "venv", $venvDir) -WorkingDirectory $repoRoot
        }

        Invoke-CheckedCommand -FilePath $venvPython -Arguments @("-m", "pip", "install", "--upgrade", "pip", "build", "twine") -WorkingDirectory $repoRoot
        Invoke-CheckedCommand -FilePath $venvPython -Arguments @("release_metadata.py", "write-version", "--version", $version) -WorkingDirectory $scratchProjectDir
        Invoke-CheckedCommand -FilePath $venvPython -Arguments @("-m", "build") -WorkingDirectory $scratchProjectDir

        $distDir = Join-Path $scratchProjectDir "dist"
        $distFiles = Get-ChildItem -LiteralPath $distDir -File
        if (-not $distFiles) {
            throw "MCP preflight build did not produce any artifacts in $distDir"
        }

        $twineArgs = @("-m", "twine", "check") + ($distFiles | ForEach-Object { $_.FullName })
        Invoke-CheckedCommand -FilePath $venvPython -Arguments $twineArgs -WorkingDirectory $scratchProjectDir

        $wheel = $distFiles | Where-Object { $_.Extension -eq ".whl" } | Select-Object -First 1
        if (-not $wheel) {
            throw "MCP preflight build did not produce a wheel artifact."
        }

        Invoke-CheckedCommand -FilePath $venvPython -Arguments @("-m", "pip", "install", "--force-reinstall", $wheel.FullName) -WorkingDirectory $scratchProjectDir
        Invoke-CheckedCommand -FilePath $venvPython -Arguments @("-c", "import sniffy_mcp; print(sniffy_mcp.__version__)") -WorkingDirectory $scratchProjectDir
    }

    Invoke-CheckedCommand -FilePath "git" -Arguments @("tag", "-a", $tag, "-m", "Sniffy MCP $version") -WorkingDirectory $repoRoot
    if (-not $SkipPush) {
        Invoke-CheckedCommand -FilePath "git" -Arguments @("push", $Remote, $tag) -WorkingDirectory $repoRoot
    }

    Write-Host "MCP release tag prepared: $tag"
    if ($SkipPush) {
        Write-Host "Tag was created locally only. Push it with: git push $Remote $tag"
    }
}
finally {
    Pop-Location
}