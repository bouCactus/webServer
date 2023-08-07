<!DOCTYPE html>
<html>
<head>
    <title>Name and Age</title>
</head>
<body>
    <form action="" method="GET">
        <label for="name">Name:</label>
        <input type="text" name="name" id="name" required><br><br>

        <label for="age">Age:</label>
        <input type="number" name="age" id="age" required><br><br>

        <input type="submit" value="Submit">
    </form>

    <?php
    if ($_SERVER['REQUEST_METHOD'] === 'GET' || 1) {
        // Check if the form has been submitted
        if (isset($_GET['name']) && isset($_GET['age'])) {
            // Retrieve the submitted name and age
            $name = $_GET['name'];
            $age = $_GET['age'];

            // Display the output
            echo '<p style="color: green;">The age of ' . $name .  ' is: ' . $age . '</p>';
        }
    }
    ?>
</body>
</html>