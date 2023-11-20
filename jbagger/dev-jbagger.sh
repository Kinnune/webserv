#!/bin/bash

# Merge dev into jbagger
git checkout jbagger
git pull origin dev
git merge dev

# Check for conflicts
if [ $? -ne 0 ]; then
    echo "Conflict encountered. Please resolve conflicts and commit the changes."
    exit 1
fi

# Push changes to jbagger
git commit -m "Merge 'dev' into 'jbagger'"
git push origin jbagger
echo "Merge successful!"