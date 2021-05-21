import requests
import json
from time import sleep

player = int(input('Введите ID игрока: '))
uid    = input('Введите ID сессии (1337): ')
port   = input('Введите порт сервера (8080): ')

if uid == '':
    uid = '1337'

if port == '':
    port = '8080'

server = 'http://localhost:' + port

while True:
    move = input('Введите ваш ход: ')

    if move == '':
        print('Пропускаю ваш ход.')
    else:
        req = {
            'player': player,
            'uid': uid,
            'move': move
        }
        r = requests.post(server, data=json.dumps(req))
        j = r.json()
        status = j['status']
        if status == 'not-your-move':
            print('Это был не ваш ход')
        elif status == 'success':
            print('Успешно отправлен ход!')
        else:
            print('Неизвестная ошибка')
            print(r)
            print(j)
            exit()

    print('Ждём ход от другого игрока', end='')
    while True:
        r = requests.get(server, params = {'uid': uid, 'player': player})
        j = r.json()
        status = j['status']

        if status == 'no-move-from-other-player':
            print('.', end='', flush=True)
            sleep(1)
            continue
        print()
        if status == 'move':
            other_player = j['player']
            other_move = j['move']
            print(f'Игрок {other_player} походил: {other_move}')
        elif status == 'empty':
            print('Ходов никто не делал. Ходите первым!')
        else:
            print('Неизвестная ошибка')
            print(r)
            print(j)
            exit()
        break

    print()
    
