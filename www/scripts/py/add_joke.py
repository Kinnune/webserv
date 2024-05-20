import os		# environ
import sys		# stdin
import hashlib	# sha256

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

def get_username_from_session_id(session_id):
	# Get the session folder path
	session_folder = os.path.join("sessions", session_id)
	# Check if session folder exists
	if not os.path.isdir(session_folder):
		print("Error: Session folder does not exist")
		return None

	# Get the username from the session file
	session_file = os.path.join(session_folder, "session.txt")
	with open(session_file, 'r') as file:
		username = file.read()
	
	return username

#-------------------------------------------------------------------------------

def main:
	print("This is ADD JOKE script")
	env = os.environ
	session_id = get_session_id(env)
	joke = None

	# Get the joke from stdin
	for line in sys.stdin:
		key, value = line.split('=')
		if key == 'joke':
			joke = value
			break
	
	# Check if the joke is not empty
	if joke is None:
		print("Error: Joke cannot be empty")
		return
	
	# Get the username from the session ID
	username = get_username_from_session_id(session_id)

	# Check if the username is not empty
	if username is None:
		print("Error: User not logged in")
		return
	
	# Add the joke to the user's joke file
	user_folder = os.path.join("database", username)
	jokes_file = os.path.join(user_folder, "jokes.txt")
	with open(jokes_file, 'a') as file:
		file.write(joke + '\n')
	
#-------------------------------------------------------------------------------

if __name__ == "__main__":
	main()