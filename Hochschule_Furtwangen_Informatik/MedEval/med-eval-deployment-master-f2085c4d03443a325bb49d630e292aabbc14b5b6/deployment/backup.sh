#!/bin/bash
ansible-playbook -i hosts backup_database.yml  -k -u root
