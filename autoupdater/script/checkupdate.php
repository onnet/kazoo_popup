<?php

$app = $_GET['app'];
$platform = $_GET['platform'];
$user_version = $_GET['ver'];
$mask = $app."_".$platform."_*.zip";
$versionRegexpPattern = "@".$app."_".$platform."_(.*).zip@i";

function getDirectoryList ($directory, $mask = "*") 
{
  $results = array();

  $handler = opendir($directory);

  while ($file = readdir($handler)) {
    if ($file != "." && $file != ".." && fnmatch($mask, $file)) {
      $results[] = $file;
    }
  }

  closedir($handler);

  return $results;
}

function getVersion($filename, $pattern)
{
  if (preg_match($pattern, $filename, $matches))
    return $matches[1];
  return "0";
}

$list = getDirectoryList("updates/".$app, $mask);

if (empty($list))
  return "";

foreach ($list as &$file) {
   $version = getVersion($file, $versionRegexpPattern);
   if (empty($lastVersion) || version_compare($lastVersion, $version, "<")) {
    $lastFileName = $file;
    $lastVersion = $version;
   }
 }

if (isset($lastFileName) && version_compare($lastVersion, $user_version, ">"))
  printf("http://%s/updates/%s/%s", $_SERVER["HTTP_HOST"], $app, $lastFileName);

?>
