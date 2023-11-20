#!/bin/bash

# Merge jbagger into dev
git checkout dev
git pull origin jbagger
git merge jbagger

# Check for conflicts
if [ $? -ne 0 ]; then
    echo "Conflict encountered. Please resolve conflicts and commit the changes."
    exit 1
fi

# Push changes to dev
git commit -m "Merge 'jbagger' into 'dev'"
git push origin dev
echo "Merge successful!"