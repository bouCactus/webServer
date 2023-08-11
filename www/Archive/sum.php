<?php
    header("Status: 200 Ok");
?>
<!DOCTYPE html>
<html>
<head>
    <title>Addition Form</title>
</head>
<body>
    <form action="sum.php" method="GET">
        <label for="num1">Number 1:</label>
        <input type="number" name="num1" id="num1" required><br><br>

        <label for="num2">Number 2:</label>
        <input type="number" name="num2" id="num2" required><br><br>

        <input type="submit" value="Calculate">
    </form>

    <?php
    if (isset($_GET['num1']) && isset($_GET['num2'])) {
        // Retrieve the submitted numbers
        $num1 = $_GET['num1'];
        $num2 = $_GET['num2'];

        // Calculate the sum
        $sum = $num1 + $num2;

        // Display the result
        echo '<p style="color: green;">The sum of ' . $num1 . ' and ' . $num2 . ' is: ' . $sum . '</p>';
    }
    ?>
</body>
</html>