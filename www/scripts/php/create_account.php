<?php

if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    // Process form data
    $username = $_POST["username"] ?? "";
    $password = $_POST["password"] ?? "";

	// DEBUG
	echo "Username: $username\nPassword: $password\n";

	$contentType = $_SERVER['HTTP_CONTENT_TYPE'] ?? '';
	echo "Content-Type: $contentType\n";

	$cookie = $_SERVER['HTTP_COOKIE'] ?? '';
	echo "Cookie: $cookie\n";

    if (!empty($username) && !empty($password))
	{
        // Example: Store in a file (for demonstration purposes)
        $file = fopen("user_accounts.txt", "a");
        if ($file)
		{
            fwrite($file, "Username: $username, Password: $password\n");
            fclose($file);
            echo "<p>Account created successfully!</p>";
        }
		else
		{
            echo "<p>Error creating account. Please try again.</p>";
        }
    }
	else
	{
        echo "<p>Please enter both username and password.</p>";
    }
}
else
{
    // Show account creation form
    echo "<html><head><title>Account Creation</title></head><body>";
    echo "<form method=\"post\">";
    echo "Username: <input type=\"text\" name=\"username\"><br>";
    echo "Password: <input type=\"password\" name=\"password\"><br>";
    echo "<input type=\"submit\" value=\"Create Account\">";
    echo "</form>";
    echo "</body></html>";
}

?>