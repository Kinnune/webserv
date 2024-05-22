import os
import cgi

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
	# print("Session ID: ", session_id)
	# print("\n")
	# print("Session Folder: ", session_folder)
	# print("\n")
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

def load_jokes(user):
	user_folder = os.path.join("database", user)
    jokes_file = os.path.join(user_folder, 'jokes.txt')
    jokes = []
    if os.path.exists(jokes_file):
        with open(jokes_file, 'r') as file:
            jokes = file.readlines()
    return jokes

#-------------------------------------------------------------------------------

def save_jokes(user, jokes):
    user_folder = os.path.join("database", user)
	jokes_file = os.path.join(user_folder, 'jokes.txt')
    with open(jokes_file, 'w') as file:
        file.writelines(jokes)

#-------------------------------------------------------------------------------

def delete_joke(user, joke_id):
    jokes = load_jokes(user)
    jokes = [joke for joke in jokes if not joke.startswith(f"{joke_id}:")]
    save_jokes(user, jokes)

#-------------------------------------------------------------------------------

def generate_html(user, jokes):
    html_content = '''<!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" href="profile.css">
        <title>Profile</title>
    </head>
    <body>
        <div class="container">
            <div class="profile_box">
                <div class="image_box"></div>
                <div class="name">'''
	html_content += user
	html_content += '''</div>
            </div>
            <div class="line"></div>
            <form id="addJokeForm" method="post" action="py/add_joke.py">
                <div class="addJoke">
                    <input type="text" id="newJoke" name="newJoke" required>
                    <button class="button" type="submit">Add Joke</button>
                </div>
            </form>
            <div class="buttons">
            </div>
            <div class="line"></div>
            <div class="jokes">'''
    
    for joke in jokes:
        joke_id, joke_text = joke.split(':', 1)
        html_content += f'''
                <div class="line"></div>
                <div class="jokebox" id="{joke_id}">
                    <div class="joke">{joke_text}</div>
                    <button onclick="deleteJoke({joke_id})">Delete</button>
                </div>'''
    
    html_content += '''
            </div>
        </div>
	<script src="profile.js"></script>
    </body>
    </html>'''
    return html_content

#-------------------------------------------------------------------------------

def main():
	env = os.environ
    method = env['REQUEST_METHOD']
	user = get_username_from_session_id(get_session_id(env))
    if method == 'DELETE':
        input_data = cgi.FieldStorage()
        joke_id = input_data.getvalue('joke_id')
        if joke_id:
            delete_joke(user, joke_id)
    
    jokes = load_jokes()
    html_content = generate_html(user, jokes)

    print(html_content)

if __name__ == "__main__":
    main()