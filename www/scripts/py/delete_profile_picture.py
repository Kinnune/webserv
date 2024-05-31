import os
import sys
import cgi
import cgitb; cgitb.enable()  # for troubleshooting

def main():
    # Only allow DELETE method
    if os.environ['REQUEST_METHOD'] != 'DELETE':
        print("Status: 405 Method Not Allowed")
        print("Content-Type: text/html")
        print()
        print("<html><body><p>Method Not Allowed</p></body></html>")
        return

    # Define the directory and file to delete
    username = os.environ['USERNAME']  # or however you get the current user
    user_dir = os.path.join('database', username)
    filepath = os.path.join(user_dir, 'profile_picture.png')

    # Delete the file if it exists
    if os.path.exists(filepath):
        os.remove(filepath)
		print("Status: 200 OK")
		print("Content-Type: text/html")
		print()
		print("<html><body><p>Profile picture deleted</p></body></html>")
    else:
		print("Status: 404 Not Found")
		print("Content-Type: text/html")
		print()
		print("<html><body><p>Profile picture not found</p></body></html>")

if __name__ == '__main__':
    main()