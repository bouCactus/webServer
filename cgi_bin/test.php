<?php
   $input = file_get_contents('php://input');
   parse_str($input, $postData);
   
   $user = isset($postData['user']) ? $postData['user'] : '';
   $name = isset($postData['name']) ? $postData['name'] : '';
   
   echo "User: $user, Name: $name";
?>