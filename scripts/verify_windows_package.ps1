param(
    [Parameter(Mandatory = $true)]
    [string] $Version,
    [Parameter(Mandatory = $true)]
    [string] $PhpVersion,
    [Parameter(Mandatory = $true)]
    [ValidateSet('ts', 'nts')]
    [string] $ThreadSafety
)

$ErrorActionPreference = 'Stop'
$pattern = "php_pinpoint_php-$Version-$PhpVersion-$ThreadSafety-*-x86_64.zip"
$packages = @(Get-ChildItem -Path artifacts -Filter $pattern -File)
if ($packages.Count -ne 1) {
    throw "Expected one Windows package matching $pattern"
}

$package = $packages[0]
$packageDirectory = Join-Path $env:RUNNER_TEMP "pinpoint-package-$PhpVersion-$ThreadSafety"
Expand-Archive -Path $package.FullName -DestinationPath $packageDirectory -Force
$dll = Join-Path $packageDirectory "$($package.BaseName).dll"
if (-not (Test-Path -LiteralPath $dll -PathType Leaf)) {
    throw 'The ZIP does not contain the PIE-compatible DLL name'
}

# The official builder creates a unique subdirectory containing the target PHP.
$phpBinaries = @(Get-ChildItem -Path build -Recurse -Filter php.exe -File |
    Where-Object { $_.Directory.Name -eq 'php-bin' })
if ($phpBinaries.Count -ne 1) {
    throw 'Cannot identify the PHP runtime used to build the extension'
}
$php = $phpBinaries[0].FullName
$actual = & $php -n -d "extension=$dll" -r 'echo phpversion("pinpoint_php");'
if ($LASTEXITCODE -ne 0 -or "$actual" -ne $Version) {
    throw "Packaged DLL failed its version check: expected $Version, received $actual"
}
Write-Host "Verified $($package.Name): extension version $Version"
