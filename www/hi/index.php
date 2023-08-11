<?php
// sleep(10);
// echo "Hello World!";
header("Status: 200 Ok");

header("Content-Type: text/html");

// Generate the content of your response
$responseContent = "<html><body><h1>Hello, CGI PHP!</h1>";

// Calculate the length of the response content in bytes
$contentLength = strlen($responseContent);

// Set the Content-Length header to the calculated length
// header("Content-Length: " . $contentLength);

// Output the response content
//echo $responseContent;

foreach ($_SERVER as $key => $value) {
    echo "<p>$key: $value</p>";
}
echo "</body></html>"
?>