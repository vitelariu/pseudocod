param (
    [switch]$Clean,
    [switch]$Verbose,
    [switch]$Quiet,
    [switch]$Help
)

$BuildDir = "build"
$CmakeBuildType = "Release"

function Show-Help {
    @"
Usage: build.ps1 [options]
Options:
  -Clean         Clean the build directory before building
  -Verbose       Show additional output
  -Quiet         Suppress all output
  -Help          Show this help message and exit
Example:
  ./build.ps1 -Clean -Verbose
"@
}

function Invoke-Build {
    Try {
        Write-Host "Configuring the build..."
        cmake .. -DCMAKE_BUILD_TYPE=$CmakeBuildType
        Write-Host "Building the project..."
        make
        Write-Host "Build successful! Binary is located in $BuildDir/bin/"
    } Catch {
        Write-Host "Build failed. Exiting."
        exit 1
    }
}

function Clean-Build {
    if (Test-Path $BuildDir) {
        Write-Host "Cleaning the build directory..."
        Remove-Item -Recurse -Force $BuildDir
    } else {
        Write-Host "Build directory not found. No need to clean."
    }
}

function Initialize-Environment {
    # Check if CMake is installed
    if (-not (Get-Command "cmake" -ErrorAction SilentlyContinue)) {
        Write-Host "CMake not found. Please ensure that CMake is installed and available in your PATH."
        exit 1
    }

    # Create build directory if it does not exist
    if (-not (Test-Path $BuildDir)) {
        Write-Host "Creating build directory..."
        New-Item -ItemType Directory -Path $BuildDir | Out-Null
    }

    # Navigate to build directory
    Set-Location $BuildDir
}

function Set-QuietMode {
    $script:ErrorActionPreference = "SilentlyContinue"
    $script:WarningPreference = "SilentlyContinue"
    $script:VerbosePreference = "SilentlyContinue"
    $script:DebugPreference = "SilentlyContinue"
    $script:ProgressPreference = "SilentlyContinue"
}

if ($Help) {
    Show-Help
    exit
}

if ($Quiet) {
    Set-QuietMode
}

Initialize-Environment

if ($Clean) {
    Clean-Build
}

Invoke-Build

# Return to original directory
Set-Location ..

if ($Verbose) {
    Write-Host "Script execution complete."
}
