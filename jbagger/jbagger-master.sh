#!/bin/bash

# Merge jbagger into master
git checkout master
git pull origin jbagger
git merge jbagger

# Check for conflicts
if [ $? -ne 0 ]; then
    echo "Conflict encountered. Please resolve conflicts and commit the changes."
    exit 1
fi

# Push changes to master
git push origin master
echo "Merge successful!"