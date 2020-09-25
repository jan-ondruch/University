#!/bin/bash
ansible-playbook -i hosts hardening.yml  -k -u root
