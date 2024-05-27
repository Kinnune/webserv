def generate_simple_form():
    print('<!DOCTYPE html>')
    print('<html lang="en">')
    print('<head>')
    print('\t<meta charset="UTF-8">')
    print('\t<meta http-equiv="X-UA-Compatible" content="IE=edge">')
    print('\t<meta name="viewport" content="width=device-width, initial-scale=1.0">')
    print('\t<title>Test Form</title>')
    print('</head>')
    print('<body>')
    print('\t<form id="uploadForm" method="post" action="/py/upload_file.py" enctype="multipart/form-data">')
    print('\t\t<input type="file" name="file" id="file">')
    print('\t\t<button type="submit">Upload</button>')
    print('\t</form>')
    print('</body>')
    print('</html>')

if __name__ == "__main__":
    generate_simple_form()