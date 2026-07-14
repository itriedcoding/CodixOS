#!/bin/bash
# CodixOS Vercel Deployment Script
# Deploy website to Vercel

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
    echo -e "${GREEN}  Vercel Deployment Script${NC}"
    echo ""
}

check_vercel() {
    if ! command -v vercel &> /dev/null; then
        echo -e "${YELLOW}Vercel CLI not found. Installing...${NC}"
        npm install -g vercel
    fi
}

check_node() {
    if ! command -v node &> /dev/null; then
        echo -e "${RED}Error: Node.js is not installed${NC}"
        echo "Please install Node.js from https://nodejs.org"
        exit 1
    fi
    
    if ! command -v npm &> /dev/null; then
        echo -e "${RED}Error: npm is not installed${NC}"
        exit 1
    fi
}

install_deps() {
    echo -e "${YELLOW}Installing website dependencies...${NC}"
    cd website
    npm install
    cd ..
}

build_website() {
    echo -e "${YELLOW}Building website...${NC}"
    cd website
    npm run build
    cd ..
}

deploy_vercel() {
    echo -e "${YELLOW}Deploying to Vercel...${NC}"
    cd website
    
    # Check if already logged in
    if ! vercel whoami > /dev/null 2>&1; then
        echo -e "${YELLOW}Please login to Vercel:${NC}"
        vercel login
    fi
    
    # Deploy
    vercel --prod
    
    cd ..
    echo -e "${GREEN}Deployment complete!${NC}"
}

print_summary() {
    echo ""
    echo -e "${GREEN}=====================================${NC}"
    echo -e "${GREEN}   Website Deployed!${NC}"
    echo -e "${GREEN}=====================================${NC}"
    echo ""
    echo -e "Your website is now live on Vercel!"
    echo ""
    echo -e "Next steps:"
    echo -e "  1. Check your deployment URL"
    echo -e "  2. Configure custom domain (optional)"
    echo -e "  3. Set up environment variables"
    echo ""
}

main() {
    print_banner
    check_node
    check_vercel
    install_deps
    build_website
    deploy_vercel
    print_summary
}

main "$@"
