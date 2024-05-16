import os		# path, mkdir, environ
import sys		# stdin
import hashlib	# sha256

#-------------------------------------------------------------------------------

def generate_response_user_exists():
	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("\t<meta charset=\"UTF-8\">")
	print("\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
	print("\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
	print("\t<link rel=\"stylesheet\" href=\"jokebook/login_err.css\">")
	print("\t<title>JokeBook</title>")
	print("</head>")
	print("<body>")
	print("\t<div class=\"container\">")
	print("\t\t<div class=\"error_message\">User already exists! Try another user or logging in instead.</div>")
	print("\t\t<div class=\"title\">JokeBook</div>")
	print("\t\t<div class=\"login-container\">")
	print("\t\t\t<form id=\"loginForm\" method=\"post\" action=\"py/login.py\">")
	print("\t\t\t\t<div class=\"name\">")
	print("\t\t\t\t\t<label for=\"username\">Username:</label>")
	print("\t\t\t\t\t<input type=\"text\" id=\"username\" name=\"username\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<div class=\"password\">")
	print("\t\t\t\t\t<label for=\"password\">Password:</label>")
	print("\t\t\t\t\t<input type=\"password\" id=\"password\" name=\"password\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<button type=\"submit\">Login</button>")
	print("\t\t\t\t<button type=\"submit\" formaction=\"py/create_account.py\">Create Account</button>")
	print("\t\t\t</form>")
	print("\t\t</div>")
	print("\t</div>")
	print("</body>")
	print("</html>")

#-------------------------------------------------------------------------------

def generate_response_account_created():
	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("\t<meta charset=\"UTF-8\">")
	print("\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
	print("\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
	print("\t<link rel=\"stylesheet\" href=\"jokebook/login.css\">")
	print("\t<title>JokeBook</title>")
	print("</head>")
	print("<body>")
	print("\t<div class=\"container\">")
	print("\t\t<div class=\"title\">JokeBook</div>")
	print("\t\t<div class=\"login-container\">")
	print("\t\t\t<form id=\"loginForm\" method=\"post\" action=\"py/login.py\">")
	print("\t\t\t\t<div class=\"name\">")
	print("\t\t\t\t\t<label for=\"username\">Username:</label>")
	print("\t\t\t\t\t<input type=\"text\" id=\"username\" name=\"username\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<div class=\"password\">")
	print("\t\t\t\t\t<label for=\"password\">Password:</label>")
	print("\t\t\t\t\t<input type=\"password\" id=\"password\" name=\"password\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<button type=\"submit\">Login</button>")
	print("\t\t\t\t<button type=\"submit\" formaction=\"py/create_account.py\">Create Account</button>")
	print("\t\t\t</form>")
	print("\t\t</div>")
	print("\t</div>")
	print("</body>")
	print("</html>")

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

def check_if_user_exists(username, password):
	# Get user folder path
	user_folder = os.path.join("database", username)
	# Check if user folder exists
	return os.path.isdir(user_folder)

#-------------------------------------------------------------------------------

def create_account(username, password):
	# Create user folder
	user_folder = os.path.join("database", username)
	try:
		os.mkdir(user_folder)
	except OSError as error:  
		print(error)
	
	# Create file named credentials.txt in user folder
	credentials_file = os.path.join(user_folder, "credentials.txt")
	
	# Write username and hashed password to credentials file
	with open(credentials_file, 'w') as file:
		file.write("pw=" + hash(password))
	# print("New user created", username)

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
		# print("Error: Username or password not found in input")
		return

	user_exists = check_if_user_exists(username, password)

	if user_exists:
		# print("User [", username, "] already exists...")
		generate_response_user_exists()
	else:
		# print("Creating new user [", username, "]...")
		create_account(username, password)
		generate_response_account_created()
	

#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main()