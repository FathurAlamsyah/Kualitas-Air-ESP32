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

// get data terakhir yang masuk ke server antares
function getData()
{
  header("Content-Type: application/json");
  global $client, $headers;
  $response = $client->get(CONFIG_ANTARES_URL . '/la', ['headers' => $headers])->getBody()->getContents();
  echo $response;
}

getData();