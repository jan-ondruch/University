#!/bin/bash
ansible-playbook -i hosts restore_database.yml  -k -u root
