#!/bin/bash
ansible-playbook -i hosts reboot.yml  -k -u root
