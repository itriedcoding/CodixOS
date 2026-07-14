#!/bin/bash
# CodixOS GitHub Deployment Script
# Deploy the project to GitHub

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

print_banner() {
    echo -e "${CYAN}"
    echo "        _____      _               ____   _____ "
    echo "       / ____|    | |             |  _ \ / ____|"
    echo "      | |     ___ | | ___  _ __   | |_) | (___  "
    echo "      | |    / _ \| |/ _ \| '__|  |  _ < \\___ \\ "
    echo "      | |___| (_) | | (_) | |     | |_) |____) |"
    echo "       \\_____\\___/|_|\\___/|_|     |____/|_____/ "
    echo -e "${NC}"
    echo -e "${GREEN}  GitHub Deployment Script${NC}"
    echo ""
}

check_git() {
    if ! command -v git &> /dev/null; then
        echo -e "${RED}Error: Git is not installed${NC}"
        exit 1
    fi
    
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        echo -e "${YELLOW}Initializing git repository...${NC}"
        git init
    fi
}

check_remote() {
    if ! git remote get-url origin > /dev/null 2>&1; then
        echo -e "${YELLOW}No remote configured.${NC}"
        echo "Please add your GitHub repository remote:"
        echo "  git remote add origin https://github.com/YOUR_USERNAME/CodixOS.git"
        echo ""
        echo "Or create a new repository at:"
        echo "  https://github.com/new"
        echo ""
        read -p "Enter your GitHub repository URL: " repo_url
        if [ -n "$repo_url" ]; then
            git remote add origin "$repo_url"
        else
            echo -e "${RED}No repository URL provided${NC}"
            exit 1
        fi
    fi
}

prepare_commit() {
    echo -e "${YELLOW}Preparing commit...${NC}"
    
    # Add all files
    git add .
    
    # Check if there are changes
    if git diff --cached --quiet; then
        echo -e "${YELLOW}No changes to commit${NC}"
        return 1
    fi
    
    # Create commit
    git commit -m "feat: Initial release of CodixOS v1.0.0

- Custom kernel with memory management
- Feature-rich shell with 40+ commands
- System utilities (ls, cat, cp, mv, etc.)
- Package manager
- Desktop environment with GUI apps
- ISO builder
- Documentation and website"
    
    echo -e "${GREEN}Commit prepared${NC}"
}

push_to_github() {
    echo -e "${YELLOW}Pushing to GitHub...${NC}"
    
    # Get current branch
    branch=$(git branch --show-current)
    if [ -z "$branch" ]; then
        branch="main"
    fi
    
    # Push
    git push -u origin "$branch"
    
    echo -e "${GREEN}Successfully pushed to GitHub!${NC}"
}

print_summary() {
    echo ""
    echo -e "${GREEN}=====================================${NC}"
    echo -e "${GREEN}   Deployment Complete!${NC}"
    echo -e "${GREEN}=====================================${NC}"
    echo ""
    echo -e "Your code is now on GitHub!"
    echo ""
    echo -e "Next steps:"
    echo -e "  1. Visit your repository on GitHub"
    echo -e "  2. Set up GitHub Pages for documentation"
    echo -e "  3. Deploy website to Vercel"
    echo -e "  4. Create a release with ISO"
    echo ""
    echo -e "To deploy website to Vercel:"
    echo -e "  1. Install Vercel CLI: ${CYAN}npm i -g vercel${NC}"
    echo -e "  2. Login: ${CYAN}vercel login${NC}"
    echo -e "  3. Deploy: ${CYAN}cd website && vercel${NC}"
    echo ""
}

main() {
    print_banner
    check_git
    check_remote
    prepare_commit
    push_to_github
    print_summary
}

main "$@"
