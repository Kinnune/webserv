// Description: This file is used to upload files to the server.

// PHP tag to start the code
<?php

// Check if the submit button is clicked
if (isset($_POST['submit']))
{
	// Get the file from the form
	$file = $_FILES['file'];

	/*
	* $file = array('name' => 'file.txt',
	*				'type' => 'text/plain',
	*				'tmp_name' => '/tmp/php/php1h4j1o',
	*				'error' => 0,
	*				'size' => 123456);
	*
	* To check the file details, use the following code:
	* print_r($file);
	*/

	// Get the file details
	$fileName = $file['name'];
	$fileType = $file['type'];
	$fileTmpName = $file['tmp_name'];
	$fileError = $file['error'];
	$fileSize = $file['size'];

	/*
	* Optional way of getting, for example, file name:
	* $fileName = $_FILES['file']['name'];
	*/

	// Split the file name and the file extension
	$fileExt = explode('.', $fileName);

	// Get the file extension in lowercase
	$fileActualExt = strtolower(end($fileExt));

	// Allowed file extensions
	$allowed = array('jpg', 'jpeg', 'png', 'pdf');

	// Check if the file extension is allowed
	if (in_array($fileActualExt, $allowed))
	{
		// Check if there is an error
		if ($fileError === 0)
		{
			// Check if the file size is less than 1MB
			if ($fileSize < 1000000)
			{
				// Create a unique file name to prevent overwriting
				//	- uniqid() generates a unique ID based on the current time in microseconds
				$newFileName = uniqid('', true) . "." . $fileActualExt;

				// Set the file destination
				$fileDestination = 'uploads/' . $newFileName;

				// Move the file to the destination
				move_uploaded_file($fileTmpName, $fileDestination);

				// Redirect to the index page
				header("Location: index.php?uploadsuccess");
			}
			else
			{
				echo "Your file is too big!";
			}
		}
		else
		{
			echo "There was an error uploading your file!";
		}
	}
	else
	{
		echo "You cannot upload files of this type!";
	}
}