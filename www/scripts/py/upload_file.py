import cgi
import os
import cgitb
import sys
import fileinput

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
	print('\t\t<a href="/py/profile.py">')
	print('\t\t\t<button class="button">Back to Profile</button>')
	print('\t\t</a>')
	print('\t</div>')
	print('</body>')
	print('</html>')


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

def get_username(session_id):

	# Get the username associated with the session ID
	username = None
	if session_id:
		session_dir = os.path.join("database", "sessions", session_id)
		session_file = os.path.join(session_dir, "session.txt")
		if os.path.isfile(session_file):
			with open(session_file, 'r') as file:
				username = file.read().strip()
	
	return username


#-------------------------------------------------------------------------------

def save_uploaded_file(upload_dir, filename):

	# Sanitize the filename
	filename = os.path.basename(file_item.filename)
	filepath = os.path.join(upload_dir, filename)
	
	# for line in fileinput.input():
	# Save the file
	with open(filepath, 'wb') as f:
		f.write(file_item.file.read())	
	return filename
	
	return None


#-------------------------------------------------------------------------------

def log_debug(message):
	with open("database/DEBUG.txt", 'a') as file:
		file.write(message + '\n')


#-------------------------------------------------------------------------------

def write_env_to_file(file_path):
	# Open the file in write mode
	with open(file_path, 'w') as file:
		# Iterate over each key-value pair in os.environ
		for key, value in os.environ.items():
			# Write the key and value to the file
			file.write("{}: {}\n".format(key, value))


#-------------------------------------------------------------------------------

def print_body():
	for line in fileinput.input():
		print(line)
		print('<br>')


#-------------------------------------------------------------------------------

def save_uploaded_file(upload_dir, filename, body):

	filepath = os.path.join(upload_dir, filename)

	with open(filepath, 'wb') as f:
		for line in body:
			# f.write(line.encode())
			f.write(line)


#-------------------------------------------------------------------------------

def main():

	# Variables
	env = os.environ

	# Write the environment variables to a file
	write_env_to_file("database/ENVIRONMENT.txt")

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

	# Get the user directory
	user_dir = os.path.join("database", username)
	if not os.path.isdir(user_dir):
		generate_html("User directory does not exist.")
		return

	# Get the upload directory
	upload_dir = os.path.join(user_dir, "uploads")
	
	# Create the upload directory if it doesn't exist
	if not os.path.isdir(upload_dir):
		os.mkdir(upload_dir)
	
	# print_body()

	body = []
	filename = None
	boundary = None

	for line in fileinput.input():
		body.append(line)
		# print(line + '<br>')

	# Get the boundary
	for line in body:
		if "--" in line:
			boundary = line
			break
	
	# Get the filename
	for line in body:
		if "Content-Disposition" in line:
			filename = line.split('filename="')[1].split('"')[0]
			break

	content = []
	empty_line_found = False

	for line in body:
		stripped_line = line.strip()
		if empty_line_found:
			if boundary.strip() in stripped_line:
				break
			content.append(line)
		elif stripped_line == "":
			empty_line_found = True

	# for line in content:
	# 	print(line + '<br>')

	# print("Boundary: " + boundary + '<br>')
	# print("Filename: " + filename + '<br>')

	# Save the uploaded file
	save_uploaded_file(upload_dir, filename, content)

	# Generate the HTML
	generate_html("File uploaded successfully.")

#-------------------------------------------------------------------------------

if __name__ == '__main__':
	main()