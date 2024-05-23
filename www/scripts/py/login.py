import os		# environ
import sys		# stdin
import hashlib	# sha256
import shutil	# rmtree

#-------------------------------------------------------------------------------

def generate_response_profile(user_folder, username):

	# Variables
	joke_file = os.path.join(user_folder, "jokes.txt")
	jokes = []
	
	# Get jokes from file
	with open(joke_file, 'r') as file:
		jokes = file.read().strip().split('\n')
	
	# Generate response
	print('<!DOCTYPE html>')
	print('<html lang="en">')
	print('<head>')
	print('\t<meta charset="UTF-8">')
	print('\t<meta http-equiv="X-UA-Compatible" content="IE=edge">')
	print('\t<meta name="viewport" content="width=device-width, initial-scale=1.0">')
	print('\t<link rel="stylesheet" href="/jokebook/profile.css">')
	print('\t<title>Document</title>')
	print('</head>')
	print('<body>')
	print('\t<div class="container">')
	print('\t\t<div class="profile_box">')
	print('\t\t\t<div class="image_box"></div>')
	print('\t\t\t<div class="name">{}</div>'.format(username))
	print('\t\t</div>')
	print('\t\t<div class="line"></div>')
	print('\t\t<form id="addJokeForm" method="post" action="/py/add_joke.py">')
	print('\t\t\t<div class="addJoke">')
	print('\t\t\t\t<input type="text" id="newJoke" name="newJoke" required>')
	print('\t\t\t\t<button class="button" type="submit">Add Joke</button>')
	print('\t\t\t</div>')
	print('\t\t</form>')
	print('\t\t<div class="jokes">')
	
	#Generate jokes
	for i, joke in enumerate(jokes):
		print('\t\t\t<div class="line"></div>')
		print('\t\t\t<div class="jokebox">')
		print('\t\t\t\t<div class="joke" id="{}">{}</div>'.format(i, joke))
		print('\t\t\t\t<button class="deleteJokeButton" onclick="deleteJoke({})">X</button>'.format(i))
		print('\t\t\t</div>')
	
	print("\t\t</div>")
	print("\t</div>")
	print('<script src="/jokebook/profile.js"></script>')
	print("</body>")
	print("</html>")
	

#-------------------------------------------------------------------------------

def generate_response_wrong_password():
	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("\t<meta charset=\"UTF-8\">")
	print("\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
	print("\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
	print("\t<link rel=\"stylesheet\" href=\"/jokebook/login_err.css\">")
	print("\t<title>JokeBook</title>")
	print("</head>")
	print("<body>")
	print("\t<div class=\"container\">")
	print("\t\t<div class=\"error_message\">Wrong password! Try again. Or not. It's up to you.</div>")
	print("\t\t<div class=\"title\">JokeBook</div>")
	print("\t\t<div class=\"login-container\">")
	print("\t\t\t<form id=\"loginForm\" method=\"post\" action=\"/py/login.py\">")
	print("\t\t\t\t<div class=\"name\">")
	print("\t\t\t\t\t<label for=\"username\">Username:</label>")
	print("\t\t\t\t\t<input type=\"text\" id=\"username\" name=\"username\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<div class=\"password\">")
	print("\t\t\t\t\t<label for=\"password\">Password:</label>")
	print("\t\t\t\t\t<input type=\"password\" id=\"password\" name=\"password\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<button type=\"submit\">Login</button>")
	print("\t\t\t\t<button type=\"submit\" formaction=\"/py/create_account.py\">Create Account</button>")
	print("\t\t\t</form>")
	print("\t\t</div>")
	print("\t</div>")
	print("</body>")
	print("</html>")

#-------------------------------------------------------------------------------

def generate_response_nonexisting_user():
	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("\t<meta charset=\"UTF-8\">")
	print("\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
	print("\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
	print("\t<link rel=\"stylesheet\" href=\"/jokebook/login_err.css\">")
	print("\t<title>JokeBook</title>")
	print("</head>")
	print("<body>")
	print("\t<div class=\"container\">")
	print("\t\t<div class=\"error_message\">User doesn't exist! You either have the wrong name or meant to create a new account.</div>")
	print("\t\t<div class=\"title\">JokeBook</div>")
	print("\t\t<div class=\"login-container\">")
	print("\t\t\t<form id=\"loginForm\" method=\"post\" action=\"/py/login.py\">")
	print("\t\t\t\t<div class=\"name\">")
	print("\t\t\t\t\t<label for=\"username\">Username:</label>")
	print("\t\t\t\t\t<input type=\"text\" id=\"username\" name=\"username\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<div class=\"password\">")
	print("\t\t\t\t\t<label for=\"password\">Password:</label>")
	print("\t\t\t\t\t<input type=\"password\" id=\"password\" name=\"password\" required>")
	print("\t\t\t\t</div>")
	print("\t\t\t\t<button type=\"submit\">Login</button>")
	print("\t\t\t\t<button type=\"submit\" formaction=\"/py/create_account.py\">Create Account</button>")
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
            # print("Login successful for user '{username}'")
            return True
    
    # print("Login failed. Invalid username or password.")
    return False

#-------------------------------------------------------------------------------

def create_session(session_id, username):

	# Create the session folder, if it doesn't exist
	sessions_folder = "database/sessions"
	if not os.path.isdir(sessions_folder):
		os.mkdir(sessions_folder)
	

	# If session folder exists, delete it
	session_folder = os.path.join(sessions_folder, session_id)
	if os.path.isdir(session_folder):
		shutil.rmtree(session_folder)
	
	# Create the session folder
	os.mkdir(session_folder)

	# Create the session file
	session_file = os.path.join(session_folder, "session.txt")
	with open(session_file, 'w') as file:
		file.write(username)

#-------------------------------------------------------------------------------

def main():
	env = os.environ
	session_id = get_session_id(env)
	username = None
	password = None

	for line in sys.stdin:
		key_value_pairs = line.strip().split('&')
		for pair in key_value_pairs:
			key, value = pair.split('=')
			if key == 'username':
				username = value
			elif key == 'password':
				password = value
		if username and password:
			break

	if not username and not password:
		return
	
	user_folder = os.path.join("database", username)
	user_exists = check_if_user_exists(username, password)

	if user_exists:
		credentials_match = check_credentials(user_folder, password)
		if credentials_match:
			create_session(session_id, username)
			generate_response_profile(user_folder, username)
		else:
			generate_response_wrong_password()
	else:
		generate_response_nonexisting_user()


#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main()