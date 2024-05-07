<?php

if ($_SERVER["REQUEST_METHOD"] === "POST")
{
	if (isset($_POST['name']) && isset($_POST['password']))
	{
		$new_user = array(
			'name' => $_POST['name'],
			'password' => $_POST['password']
		);

		$users = get_users();
		$user_exists = false;

		foreach ($users as $user)
		{
			if ($user['name'] == $new_user['name'] && $user['password'] == $new_user['password'])
			{
				echo "mlp";
				$user_exists = true;
				$_SESSION['user_auth'] = $user;

				header("location: http://localhost/dashboard.php");
				exit();
			}
		}
	}
}