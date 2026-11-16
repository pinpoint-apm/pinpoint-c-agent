<?php
// Uses the pinned PIE 1.5.5 classes to check its actual binary selection path.
declare(strict_types=1);

use Composer\IO\BufferIO;
use Composer\Package\Loader\ArrayLoader;
use Php\Pie\Building\UnixBuild;
use Php\Pie\DependencyResolver\Package;
use Php\Pie\Downloading\DownloadedPackage;
use Php\Pie\Downloading\DownloadUrlMethod;
use Php\Pie\Platform\PrePackagedBinaryAssetName;
use Php\Pie\Platform\TargetPhp\PhpBinaryPath;
use Php\Pie\Platform\TargetPlatform;
use Symfony\Component\Process\Process;

if ($argc !== 3) {
    fwrite(STDERR, "Usage: php check_php_binary.php PIE_PHAR BINARY_TGZ\n");
    exit(2);
}
require 'phar://' . realpath($argv[1]) . '/vendor/autoload.php';

$metadata = json_decode(file_get_contents(__DIR__ . '/../composer.json'), true, 512, JSON_THROW_ON_ERROR);
$composerPackage = (new ArrayLoader())->load($metadata);
DownloadUrlMethod::PrePackagedBinary->writeToComposerPackage($composerPackage);
$package = Package::fromComposerCompletePackage($composerPackage);
$target = TargetPlatform::fromPhpBinaryPath(PhpBinaryPath::fromCurrentProcess(), 2, null);
$expectedNames = PrePackagedBinaryAssetName::packageNames($target, $package);
if (!in_array(basename($argv[2]), $expectedNames, true)) {
    throw new RuntimeException('Archive filename does not match PIE: ' . basename($argv[2]));
}

$directory = sys_get_temp_dir() . '/pinpoint-binary-' . bin2hex(random_bytes(8));
mkdir($directory);
try {
    (new PharData($argv[2]))->extractTo($directory);
    $downloaded = DownloadedPackage::fromPackageAndExtractedPath($package, $directory);
    $io = new BufferIO();
    $binary = (new UnixBuild())($downloaded, $target, [], $io);
    if (!str_contains($io->getOutput(), 'Pre-packaged binary found:')) {
        throw new RuntimeException('PIE did not select the precompiled binary');
    }
    $binary->verify();
    $check = new Process([
        PHP_BINARY, '-n', '-d', 'extension=' . $binary->filePath,
        '-r', 'echo phpversion("pinpoint_php");',
    ]);
    $check->mustRun();
    if ($check->getOutput() !== $metadata['version']) {
        throw new RuntimeException('PIE-selected binary failed to load: ' . $check->getOutput() . $check->getErrorOutput());
    }
    echo 'PIE binary selection and loading verified: ', basename($argv[2]), "\n";
} finally {
    foreach (glob($directory . '/*') as $file) {
        unlink($file);
    }
    rmdir($directory);
}
