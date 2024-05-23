import os
import cgi
import cgitb; cgitb.enable()  # for troubleshooting

def main():
    # Set the directory where the profile pictures will be stored
    form = cgi.FieldStorage()
    
    if 'profilePicture' not in form:
        print("<html><body><p>No file uploaded</p></body></html>")
        return

    file_item = form['profilePicture']

    # Check if the file was uploaded
    if file_item.filename:
        # Get the filename and create a directory for the user
        username = os.environ.get('USERNAME', 'default_user')  # Replace with actual method to get username
        user_dir = os.path.join('database', username)

        if not os.path.exists(user_dir):
            os.makedirs(user_dir)

        # Construct the file path and save the file
        file_path = os.path.join(user_dir, 'profile_picture.png')
        with open(file_path, 'wb') as fout:
            fout.write(file_item.file.read())

        # Print the success message and redirect to profile page
        print("<html><body><p>Profile picture uploaded</p></body></html>")
    else:
        print("<html><body><p>No file uploaded</p></body></html>")

if __name__ == '__main__':
    main()