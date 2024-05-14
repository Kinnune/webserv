import os
import hashlib
import sys

def hash_password(password):
    # Hash the password using SHA-256
    hashed_password = hashlib.sha256(password.encode()).hexdigest()
    return hashed_password

def check_credentials(username, password):
    # Hash the input password
    hashed_input_password = hash_password(password)
    
    # Iterate through session ID folders
    database_folder = "database"
    for session_id_folder in os.listdir(database_folder):
        credentials_file = os.path.join(database_folder, session_id_folder, "credentials.txt")
        if os.path.isfile(credentials_file):
            # Read hashed password from credentials file
            with open(credentials_file, 'r') as file:
                stored_hashed_password = file.read().strip()
            
            # Compare hashed passwords
            if stored_hashed_password == hashed_input_password:
                print("Login successful for user '{username}'")
                return True
    
    print("Login failed. Invalid username or password.")
    return False

def get_session_id(env):
    # Get the value of the HTTP_COOKIE from the environment variables
    http_cookie = env.get('HTTP_COOKIE', '')

    # Parse the session ID from the HTTP_COOKIE value
    session_id = None
    cookie_pairs = http_cookie.split(';')
    for pair in cookie_pairs:
        key_value = pair.strip().split('=')
        if len(key_value) == 2 and key_value[0] == 'session_id':
            session_id = key_value[1]
            break
    
    return session_id

def main():

	env = os.environ

	# Get the session ID from the environment variables
	session_id = get_session_id(env)

	print("Session ID: ", session_id)

	for line in sys.stdin:
		key_value_pairs = line.strip().split('&')	# Strip newline characters and split the line into key-value pairs

		# Initialize variables to store username and password
		username = None
		password = None

		# Iterate through key-value pairs to extract username and password
		for pair in key_value_pairs:
			key, value = pair.split('=')
			if key == 'username':
				username = value
			elif key == 'password':
				password = value

		# Process extracted username and password
		if username and password:
			print("Username:", username)
			print("Password:", password)
		else:
			print("Error: Username or password not found in input")

		# For simplicity, assume username is the folder name
		user_folder = os.path.join("database", session_id)

		# Create folder if user does not exist
		if not os.path.isdir(user_folder):
			print("User not found... Creating new user folder.")
			try:
				os.mkdir(user_folder)
			except OSError as error:  
				print(error)
			print("User folder created. New folder: ", user_folder)
			return
		
		# Check credentials
		print("Checking credentials for user:", username)
		check_credentials(username, password)

if __name__ == "__main__":
    main()