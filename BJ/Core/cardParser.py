from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import StaleElementReferenceException
from pathlib import Path
from bs4 import BeautifulSoup
import time

save_path = "C:/Users/ManosChatzigeorgiou/Documents/BJ/Core/tableCards.txt"
save_path_shoe = "C:/Users/ManosChatzigeorgiou/Documents/BJ/Core/shoeDistribution.txt"

cardValue = {
    "card_ace" : 1,
    "card_two" : 2,
    "card_three" : 3,
    "card_four" : 4,
    "card_five" : 5,
    "card_six" : 6,
    "card_seven" : 7,
    "card_eight" : 8,
    "card_nine" : 9,
    "card_ten" : 10,
    "card_jack" : 10,
    "card_queen" : 10,
    "card_king" : 10
}

cardSuit = {
    "card_spades" : 1,
    "card_clubs" : 2,
    "card_hearts" : 3,
    "card_diamonds" : 4
}

driver = webdriver.Firefox()
driver.close()
state = old_state = ""
sz = 52 * 8
avail = [0, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32*4]
start_time = 0

def write_card(card):
    #print(card)
    card_info_raw = str(card)
    card_info = card_info_raw.split()
    #hidden card is of form card card_with-animation_horizontal card_hidden card_red ignore it
    global state, sub
    if(card_info[1] != "card_with-animation_horizontal"):
        suit = cardSuit[card_info[1]]
        value = cardValue[card_info[2]]
        state += str(value) + ' ' + str(suit) + '\n'
        #print(value, suit)
    #with open(save_path, 'w', encoding='utf-8') as tableCards:
    #tableCards.write(value + ' ' + suit)

def save_shoe():
    with open(save_path_shoe, 'w', encoding='utf-8') as shoe:
        shoe.write(str(sz) + '\n')
        for x in avail:
            shoe.write(str(x) + ' ')

def load_shoe():
    global sz, avail
    print("Loading shoe")
    shoe = open(save_path_shoe, "r")
    raw = shoe.read()
    info = raw.split()
    sz = int(info[0])
    for i in range(11):
        avail[i] = int(info[i+1])

def load_table():
    try:
        global driver, start_time
        driver = webdriver.Firefox()
        driver.get("https://www.pokerstars.gr/casino/live/")
        time.sleep(0.5)

        if(driver.find_elements("xpath", '//*[@id="onetrust-accept-btn-handler"]')):
            driver.find_element("xpath", '//*[@id="onetrust-accept-btn-handler"]').click()
        time.sleep(0.3)

        while(not driver.find_elements(By.XPATH, '//*[@title="Live All Bets Blackjack"]')):
            driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
            time.sleep(0.15)
        driver.find_element(By.XPATH, '//*[@title="Live All Bets Blackjack"]').click()

        time.sleep(0.1)

        driver.find_element(By.ID, 'userId').send_keys("Permatimmm")
        driver.find_element(By.ID, "password").send_keys("poulisgay123")
        time.sleep(0.3)
        if(driver.find_elements("xpath", '/html/body/div[3]/div/div/section/form/div[3]/button[1]')):
            driver.find_element("xpath", '/html/body/div[3]/div/div/section/form/div[3]/button[1]').click()
        if(driver.find_elements("xpath", '/html/body/div[4]/div/div/section/form/div[3]/button[1]')):
            driver.find_element("xpath", '/html/body/div[4]/div/div/section/form/div[3]/button[1]').click()

        while(len(driver.window_handles) == 1):
            time.sleep(0.2)
        print("opened table")
        window_after = driver.window_handles[1]
        driver.minimize_window()
        driver.switch_to.window(window_after)

        while(not driver.find_elements(By.ID, "GameClient")):
            time.sleep(0.3)

        iframe = driver.find_element(By.ID, "GameClient")
        driver.switch_to.frame(iframe)
        driver.minimize_window()
        print("running")
        start_time = time.time()
    except Exception as e:
        print(str(e) + "Trying again")
        load_table()

#Program start
load_table()

idle = 0
newShoe = input("New shoe? (0/1) ")
if(newShoe == "1"):
    save_shoe()
else:
    load_shoe()

while(1):
    cards = driver.find_elements(By.ID, 'card')
    if(state != ""):
        old_state = state
    state = ""
    for card in cards:
        idle = 0
        try:
            write_card(card.get_attribute('class'))
        except StaleElementReferenceException:
            state = ""
            break
    #when table was cleaned in the middle of processing or there is nothing on table
    #and the results have not yet been passed to file
    if(state == "" and old_state != ""):
        idle += 1
        state = old_state
    #pass results to file
    if(idle > 3 and old_state != ""):
        load_shoe()
        print("Writting to file")
        with open(save_path, 'w', encoding='utf-8') as tableCards:
            tableCards.write(state)
        removed = state.split()
        for i in range(0, len(removed), 2):
            sz -= 1
            avail[int(removed[i])] -= 1
        save_shoe()
        old_state = state = ""
        if(time.time() - start_time > 460):
            print("restarting to avoid afk")
            driver.switch_to.default_content()
            driver.close()
            load_table()
    time.sleep(0.3)


