import time
from PIL import Image, ImageDraw, ImageFont
import mpv

def my_log(loglevel, component, message):
    print('[{}] {}: {}'.format(loglevel, component, message))

player = mpv.MPV(log_handler=my_log, ytdl=False, input_default_bindings=True, input_vo_keyboard=True)

# Property access, these can be changed at runtime
@player.property_observer('time-pos')
def time_observer(_name, value):
    # Here, _value is either None if nothing is playing or a float containing
    # fractional seconds since the beginning of the file.
    # print('Now playing at {:.2f}s'.format(value))
    pass
    
player.loop = False
player.fullscreen = False 
player.play("/home/pi/Videos/Neutron_Stars_Colliding_1080p-1.mp4")
player.wait_until_playing()

font = ImageFont.truetype('Roboto-Condensed.ttf', 40)

while not player.core_idle:
    time.sleep(0.5)
    overlay = player.create_image_overlay()

    for pos in range(0, 500, 5):
        ts = player.time_pos
        if ts is None:
            break

        img = Image.new('RGBA', (400, 150),  (255, 255, 255, 0))
        d = ImageDraw.Draw(img)
        d.text((10, 10), 'Hello World', font=font, fill=(0, 255, 255, 128))
        d.text((10, 60), f't={ts:.3f}', font=font, fill=(255, 0, 255, 255))

        overlay.update(img, pos=(2*pos, pos))
        time.sleep(0.05)

    overlay.remove()
    
mpv.terminate()
