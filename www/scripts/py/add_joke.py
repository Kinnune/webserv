import os			# environ
import sys			# stdin
import hashlib		# sha256
import urllib		# URL decoding

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
	
	print('<div class="menuBar">')
	print('\t\t<form id="uploadForm" method="post" action="/py/upload_file.py" enctype="multipart/form-data">')
	print('\t\t\t<input type="file" name="file" id="file">')
	print('\t\t\t<button class="button" type="submit">Upload</button>')
	print('\t\t</form>')
	print('\t<a href="/jokebook/login.html">')
	print('\t\t<button class="button logOutButton">Log Out</button>')
	print('\t</a>')
	print('</div>')

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
		if joke == "":
			continue
		print('\t\t\t<div class="line"></div>')
		print('\t\t\t<div class="jokebox">')
		print('\t\t\t\t<div class="joke" id="{}">{}</div>'.format(i, joke))
		print('\t\t\t\t<button class="deleteJokeButton" onclick="deleteJoke({})">X</button>'.format(i))
		print('\t\t\t</div>')
	
	print("\t\t</div>")
	print("\t</div>")
	print("</body>")
	print("</html>")

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
	session_folder = os.path.join("database/sessions", session_id)
	
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

def add_joke(jokes_file, joke):

	# Parse the joke from the input
	joke = joke.strip()

	# Append the joke to the jokes file
	with open(jokes_file, 'a') as file:
		file.write(joke + '\n')

#-------------------------------------------------------------------------------

def main():
	env = os.environ
	session_id = get_session_id(env)
	joke = None

	for line in sys.stdin:
		key_value_pairs = line.strip().split('&')
		for pair in key_value_pairs:
			key, value = pair.split('=')
			if key == "newJoke":
				value = urllib.unquote_plus(value)  # Decode the value
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
	add_joke(jokes_file, joke)

	generate_response_profile(user_folder, username)
	
#-------------------------------------------------------------------------------

if __name__ == "__main__":
	main()