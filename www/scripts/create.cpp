#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // For getenv()

int main()
{
	// std::string formData;

	// std::cout << "----THIS IS A SCRIPT------------------------------------" << std::endl;
	// while (std::getline(std::cin, formData))
	// {
	// 	std::cout << formData << std::endl;
	// }
	// std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << "<html><head><title>Account Creation</title></head><body>";

    // Check request method
    std::string requestMethod = getenv("REQUEST_METHOD");
    if (requestMethod == "POST") {
        // Read form data from stdin
        std::string formData;
        std::getline(std::cin, formData);


        // Example: Extract username and password
        std::string username, password;
        size_t pos = formData.find("username=");
        if (pos != std::string::npos) {
            pos += 9; // Length of "username="
            size_t endPos = formData.find("&", pos);
            if (endPos != std::string::npos) {
                username = formData.substr(pos, endPos - pos);
            }
        }
        pos = formData.find("password=");
        if (pos != std::string::npos) {
            pos += 9; // Length of "password="
            password = formData.substr(pos);
        }

        // Process the extracted data (e.g., store in a file or database)
        // Example: Store in a file (for demonstration purposes)
        std::ofstream outfile("user_accounts.txt", std::ios_base::app);
        if (outfile.is_open()) {
            outfile << "Username: " << username << ", Password: " << password << "\n";
            outfile.close();
            std::cout << "<p>Account created successfully!</p>";
        } else {
            std::cout << "<p>Error creating account. Please try again.</p>";
        }
    } else {
        // Handle GET request (show form for account creation)
        std::cout << "<form method=\"post\">";
        std::cout << "Username: <input type=\"text\" name=\"username\"><br>";
        std::cout << "Password: <input type=\"password\" name=\"password\"><br>";
        std::cout << "<input type=\"submit\" value=\"Create Account\">";
        std::cout << "</form>";
    }

    std::cout << "</body></html>";
    return 0;
}