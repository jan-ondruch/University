import urllib
import os

from BeautifulSoup import BeautifulSoup

limesurvey_download_page = 'https://www.limesurvey.org/de/stable-release'
limesurvey_download_base_url = 'https://www.limesurvey.org'
archive_file_name = 'limesurvey.tar'

path = os.path.join("/tmp", archive_file_name)

page = urllib.urlopen(limesurvey_download_page).read()
soup = BeautifulSoup(page)

url = []
for div_class in soup.findAll('div', {"class": "uk-width-1-1"}):
    for a_class in div_class.findAll('a', {"class": "btn btn-success"}):
        url.append(a_class['href'])

for element in url:
    if 'targz' in element:
        limesurvey_download_base_url += element
        break

urllib.urlretrieve(limesurvey_download_base_url, path)
