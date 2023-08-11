<?php
    header("Status: 200 Ok");
?>
<!DOCTYPE html>
<html>
<head>
    <title>Hobby and Time</title>
</head>
<body>
    <form action="" method="POST">
        <label for="hobby">Hobby:</label>
        <input type="text" name="hobby" id="hobby" required><br><br>

        <label for="time">Time:</label>
        <input type="number" name="time" id="time" required><br><br>

        <input type="submit" value="Submit">
    </form>

    <?php
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        // Check if the form has been submitted
        $input = file_get_contents('php://input');
        parse_str($input, $postData);

        $hobby = isset($postData['hobby']) ? $postData['hobby'] : '';
        $time = isset($postData['time']) ? $postData['time'] : '';
        echo '<p style="color: green;">The time of ' . $hobby .  ' is: ' . $time . '</p>';
    }
  
    // foreach ($_SERVER as $key => $value) {
    //     echo "<p>$key: $value</p>";
    // }
    ?>
</body>
</html>