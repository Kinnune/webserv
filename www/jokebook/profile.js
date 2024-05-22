function deleteJoke(jokeId)
{
	fetch('/py/delete_joke.py', {
		method: 'DELETE',
		headers: {
			'Content-Type': 'application/x-www-form-urlencoded'
		},
		body: `joke_id=${jokeId}`
	})
	.then(response => response.text())
	.then(html => {
		document.documentElement.innerHTML = html;
	});
}