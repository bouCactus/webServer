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
        if (isset($_POST['hobby']) && isset($_POST['time'])) {
            // Retrieve the submitted hobby and time
            $hobby = $_POST['hobby'];
            $time = $_POST['time'];

            // Display the output
            echo '<p style="color: green;">The time of ' . $hobby .  ' is: ' . $time . '</p>';
        }
    }
    ?>
</body>
</html>