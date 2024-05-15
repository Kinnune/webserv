import os		# path, mkdir, environ
import hashlib	# sha256
import sys		# stdin

#-------------------------------------------------------------------------------

def hash(password):
    # Hash the password using SHA-256
    hashed_password = hashlib.sha256(password.encode()).hexdigest()
    return hashed_password

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

def check_credentials(user_folder, password):
    # Hash the password
    hashed_password = hash(password)
	# Check if credentials file exists
    credentials_file = os.path.join(user_folder, "credentials.txt")
    if os.path.isfile(credentials_file):
		# Read hashed password from credentials file
        with open(credentials_file, 'r') as file:
            stored_hashed_password = file.read().strip()
		
		# Compare hashed passwords
        if stored_hashed_password == hashed_password:
            print("Login successful for user '{username}'")
            return True
    
    print("Login failed. Invalid username or password.")
    return False

#-------------------------------------------------------------------------------

def main():

	# Variables
	env = os.environ
	session_id = get_session_id(env)
	username = None
	password = None

	# Read input from standard input
	for line in sys.stdin:
		key_value_pairs = line.strip().split('&')	# Strip newline characters and split the line into key-value pairs

		# Iterate through key-value pairs to extract username and password
		for pair in key_value_pairs:
			key, value = pair.split('=')
			if key == 'username':
				username = value
			elif key == 'password':
				password = value

		# Process extracted username and password
		if username and password:
			break

	# Check if we have both username and password
	if not username and not password:
		print("Error: Username or password not found in input")
		return

	# Create folder if user does not exist
	user_folder = os.path.join("database", username)
	if not os.path.isdir(user_folder):
		try:
			os.mkdir(user_folder)
		except OSError as error:  
			print(error)
		
		# Create file named credentials.txt in user folder
		credentials_file = os.path.join(user_folder, "credentials.txt")
		
		# Write username and hashed password to credentials file
		with open(credentials_file, 'w') as file:
			file.write("pw=" + hash(password))
		print("New user created", username)
		return
	
	print("User [", username, "] already exists...")

#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main()