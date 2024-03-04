from selenium import webdriver
from selenium.webdriver.common.by import By

# Initialize a browser instance
driver = webdriver.Chrome()

# Navigate to the HTML page containing the button
driver.get("https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D3.3&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&loader=on")

# Find the button element by its XPath, ID, class, or other selector
button = driver.find_element(By.XPATH, "//input[@class='button']")

# Click the button
button.click()

# Retrieve the URL after the button click
redirected_url = driver.current_url
redirected_url += "glad.zip"
print(redirected_url)

driver.quit()