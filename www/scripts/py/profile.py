import os	

def generate_profile(user_folder, username):

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

	# Menu bar
	print('\t<div class="menuBar">')
	print('\t\t<form id="uploadForm" method="post" action="/py/upload_file.py" enctype="multipart/form-data">')
	print('\t\t\t<input type="file" name="file" id="file">')
	print('\t\t\t<button class="button" type="submit">Upload</button>')
	print('\t\t</form>')
	print('\t\t<a href="/jokebook/login.html">')
	print('\t\t\t<button class="button logOutButton">Log Out</button>')
	print('\t\t</a>')
	print('\t</div>')

	# Container
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
	print("</body>")
	print("</html>")

#-------------------------------------------------------------------------------

def generate_html(message):
	print('<!DOCTYPE html>')
	print('<html lang="en">')
	print('<head>')
	print('\t<meta charset="UTF-8">')
	print('\t<meta http-equiv="X-UA-Compatible" content="IE=edge">')
	print('\t<meta name="viewport" content="width=device-width, initial-scale=1.0">')
	print('\t<link rel="stylesheet" href="/jokebook/upload_message.css">')
	print('\t<title>JokeBook</title>')
	print('</head>')
	print('<body>')
	print('\t<div class="container">')
	print('\t\t<div class="title">JokeBook</div>')
	print('\t\t<div class="message">{}</div>'.format(message))
	print('\t\t<a href="/jokebook/profile.html">')
	print('\t\t\t<button class="button">Back to Profile</button>')
	print('\t\t</a>')
	print('\t</div>')
	print('</body>')
	print('</html>')

#-------------------------------------------------------------------------------

def get_session_id(env):
	
	# Get the session ID
	session_id = None
	if 'HTTP_COOKIE' in env:
		cookies = env['HTTP_COOKIE'].split(';')
		for cookie in cookies:
			key, value = cookie.split('=')
			if key == 'session_id':
				session_id = value
				break

	return session_id

#-------------------------------------------------------------------------------

def get_username(session_id):
	
	# Get the username associated with the session ID
	username = None
	if session_id:
		session_file = os.path.join("database", "sessions", session_id, "session.txt")
		if os.path.isfile(session_file):
			with open(session_file, 'r') as file:
				username = file.read().strip()
	
	return username

#-------------------------------------------------------------------------------

def main():

	env = os.environ

	# Get session ID
	session_id = get_session_id(env)
	if not session_id:
		generate_html("Session ID not found.")
		return
	
	# Get username
	username = get_username(session_id)
	if not username:
		generate_html("Username not found.")
		return
	
	# Get user folder
	user_folder = os.path.join("database", username)
	if not os.path.isdir(user_folder):
		generate_html("User folder not found.")
		return
	
	# Generate response
	generate_profile(user_folder, username)

#-------------------------------------------------------------------------------

if __name__ == "__main__":
	main()