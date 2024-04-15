#!/bin/bash

# Update jbagger branch
# git add .
# git commit -m "Committing changes to jbagger branch"
# git push origin jbagger

# Merge jbagger into master
git checkout master
git pull origin master
git merge jbagger

# Check for conflicts
if [ $? -ne 0 ]; then
    echo "Conflict encountered. Please resolve conflicts and commit the changes."
    exit 1
fi

# Push changes to master
git push origin master
echo "Merge successful!"

# Switch back to jbagger branch
git checkout jbagger
git pull origin jbagger