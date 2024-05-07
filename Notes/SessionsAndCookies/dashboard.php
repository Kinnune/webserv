<?php

$welcome_div;

if (isset($_SESSION['user_auth']))
{
	$users = get_users();
	$_SESSION['dash-nav'] = true;

	if (isset($_SESSION['user-created']))
	{
		$welcome_div = '
		<div class="error-div">
			<div class="box">
				<p>Welcome '.$_SESSION['user_auth']['name'].' '.$_SESSION['user_auth']['surname'].', you are a new user!</p>
			</div>
		</div>';
	}
	else
	{
		$welcome_div = '
		<div class="error-div">
			<div class="box">
				<p>Welcome back'.$_SESSION['user_auth']['name'].' '.$_SESSION['user_auth']['surname'].'!</p>
			</div>
		</div>';
	}
}
else
{
	$_SESSION['no_auth'] = true;
	header("location: http://localhost");
	exit();
}