<?php
    header("Status: 200 Ok");

    session_start();
    $input = file_get_contents('php://input');
    parse_str($input, $_POST);
 if(isset($_SESSION['username']) && !empty($_SESSION['username'])) {
    header('Location: index.php');
}
// Check if the form has been submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the submitted username and password
    $username = $_POST['username'];
    $password = $_POST['password'];

    // Check if the username and password are correct
    if ($username === 'tester' && $password === '1234') {
        // Set a cookie with the username
        setcookie('username', $username, time() + 3600); // Expires in 1 hour

        // Store the username in a session variable
        $_SESSION['username'] = $username;

        // Redirect the user to the home page
        header('Location: index.php');
        exit;
    } else {
        // Display an error message
        setcookie('error', 'Invalid username or password', time() + 30); // Expires in 1 hour
        header('Location: login.php');
    }
    header("Location: http://localhost:8080/Archive/auth_php/index.php");

}
?>