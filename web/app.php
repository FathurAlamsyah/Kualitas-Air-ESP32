<?php
require 'vendor/autoload.php';
require_once 'config.php';

use GuzzleHttp\Client;
use GuzzleHttp\Psr7\Request;

// init header antares
$headers = [
  'X-M2M-Origin' => CONFIG_API_KEY,
  'Content-Type' => 'application/json;ty=4',
  'Accept' => 'application/json'
];

// init client
$client = new Client();

// methods
