import time
from discord.ext import commands, tasks
import discord

BOT_TOKEN = 'MTE2NjA0MTI4MTQ5MTkxMDc4OA.GZhZYw.g3yifjjEeZQA-jrhx7Q_gqAFm7J-IfJDN562So'
CHANNEL_ID = 1166060701383676084

bot = commands.Bot(command_prefix = "!", intents = discord.Intents.all())

@bot.event
async def on_ready():
    print("Online")
    await bot.get_channel(CHANNEL_ID).send("Online")
    check.start()

@tasks.loop(seconds=2)
async def check():
    rtp = open("interface.txt").read()
    if(rtp != ""):
        if(float(rtp) > 100):
            await bot.get_channel(CHANNEL_ID).send("RTP: " + rtp)
        open('interface.txt', 'w').close()

bot.run(BOT_TOKEN)