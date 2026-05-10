param(
    [string]$Remote = "origin",
    [string]$Generator = "Ninja",
    [switch]$BuildCheck,
    [switch]$SkipBuildCheck,
    [switch]$SkipPush
)

. (Join-Path $PSScriptRoot "common.ps1")

Assert-ToolAvailable -ToolName "git"

if ($BuildCheck -and $SkipBuildCheck) {
    throw "Use either -BuildCheck or -SkipBuildCheck, not both."
}

$runBuildCheck = $BuildCheck
if ($SkipBuildCheck) {
    Write-Warning "-SkipBuildCheck is no longer needed because local desktop build preflight is opt-in. Use -BuildCheck when you want a local validation build."
}

$repoRoot = Get-ReleaseRepoRoot
$version = Get-ReleaseVersion -Component "desktop"
$tag = Get-ReleaseTag -Component "desktop"

Push-Location $repoRoot
try {
    Assert-GitWorktreeClean
    Assert-NotDetachedHead
    Fetch-RemoteTags -Remote $Remote
    Assert-TagMissing -Tag $tag -Remote $Remote

    if ($runBuildCheck) {
        Assert-ToolAvailable -ToolName "cmake"
        $buildDir = Join-Path (Get-ReleaseScratchRoot) "desktop"
        try {
            Invoke-CheckedCommand -FilePath "cmake" -Arguments @("-S", "LEO_sniffy", "-B", $buildDir, "-G", $Generator, "-DCMAKE_BUILD_TYPE=Release") -WorkingDirectory $repoRoot
            Invoke-CheckedCommand -FilePath "cmake" -Arguments @("--build", $buildDir, "--config", "Release", "--target", "LEO_sniffy") -WorkingDirectory $repoRoot
        }
        catch {
            throw "Local desktop build preflight failed. Re-run without -BuildCheck to tag without a local build, or configure the local Qt/CMake environment and retry.`n$($_.Exception.Message)"
        }
    }

    Invoke-CheckedCommand -FilePath "git" -Arguments @("tag", "-a", $tag, "-m", "Sniffy Desktop $version") -WorkingDirectory $repoRoot
    if (-not $SkipPush) {
        Invoke-CheckedCommand -FilePath "git" -Arguments @("push", $Remote, $tag) -WorkingDirectory $repoRoot
    }

    Write-Host "Desktop release tag prepared: $tag"
    if ($SkipPush) {
        Write-Host "Tag was created locally only. Push it with: git push $Remote $tag"
    }
}
finally {
    Pop-Location
}