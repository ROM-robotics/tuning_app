# ROM Robotics Tuning Application - အသုံးပြုပုံလမ်းညွှန်

## Build လုပ်ပုံ

```bash
cd tuning_app
mkdir -p build
cd build
cmake ..
make
```

## Run လုပ်ပုံ

### Method 1: Manual Connection (GUI မှာ ထည့်မယ်)

```bash
./tuning_app
```

App တက်လာရင် SSH tab မှာ:
1. Robot IP address ထည့်ပါ (ဥပမာ: `192.168.1.100`)
2. Password ထည့်ပါ (ဥပမာ: `mr_robot`)
3. Namespace ထည့်ပါ (Optional - ဥပမာ: `robot1`)
4. **Go** button ကို နှိပ်ပါ

### Method 2: Auto Connection (Command Line Arguments)

IP နဲ့ password ကို command line မှာ တခါတည်း ထည့်ပေးလိုက်ရင် app တက်တာနဲ့ အလိုအလျောက် connect ဖြစ်သွားပါမယ်။

#### အခြေခံအသုံးပြုပုံ:

```bash
# Short options
./tuning_app -i 192.168.1.100 -p yourpassword

# Long options
./tuning_app --ip 192.168.1.100 --password yourpassword
```

#### Namespace ပါထည့်မယ်ဆိုရင်:

```bash
./tuning_app -i 192.168.1.100 -p yourpassword -n robot1
```

#### အသုံးပြုနိုင်တဲ့ Options:

| Option | Short | Description |
|--------|-------|-------------|
| `--ip` | `-i` | Robot IP address (လိုအပ်တယ်) |
| `--password` | `-p` | Robot password (လိုအပ်တယ်) |
| `--ns` | `-n` | Robot namespace (Optional) |
| `--help` | `-h` | Help message ကြည့်ရန် |

#### Help ကြည့်ခြင်း:

```bash
./tuning_app --help
```

## Features

### SSH Tab
- **Connection Setup**: Robot နဲ့ connect လုပ်ဖို့
- **Rsync App**: Application ကို robot ဆီကို sync လုပ်ဖို့
- **Host Terminal**: Local terminal ဖွင့်ဖို့
- **Robot Terminal**: SSH နဲ့ robot terminal ကို remote access လုပ်ဖို့

### Control Tab
Robot ကို control လုပ်ဖို့ (connection ချိတ်ပြီးမှသာ အသုံးပြုနိုင်ပါတယ်)

### EKF Tab
Extended Kalman Filter tuning အတွက် (connection ချိတ်ပြီးမှသာ အသုံးပြုနိုင်ပါတယ်)

### Carto Tab
Cartographer SLAM tuning အတွက် (connection ချိတ်ပြီးမှသာ အသုံးပြုနိုင်ပါတယ်)

## လိုအပ်ချက်များ

- Qt 6.8.3 (သို့မဟုတ် ပိုမြင့်သော version)
- CMake 3.22 (သို့မဟုတ် ပိုမြင့်သော version)
- `sshpass` (Robot terminal feature အတွက်)

```bash
# sshpass install လုပ်ရန်
sudo apt install sshpass
```

## Examples

```bash
# Example 1: Production robot
./tuning_app -i 192.168.1.100 -p robot123

# Example 2: Test robot with namespace
./tuning_app -i 10.0.0.50 -p testpass -n test_robot

# Example 3: Manual connection
./tuning_app
# ပြီးရင် GUI မှာ ထည့်ပေးပါ
```

## Troubleshooting

### Connection မဖြစ်ရင်
- Robot IP က မှန်ကန်မှုရှိမရှိ စစ်ဆေးပါ
- Password မှန်မှုရှိမရှိ စစ်ဆေးပါ
- Robot မှာ ROS Bridge server run နေမနေ စစ်ဆေးပါ
- Network connection ကောင်းမကောင်း စစ်ဆေးပါ

### Tab များ အသုံးမပြုနိုင်ရင်
- SSH tab မှာ Go button နဲ့ connection အရင် လုပ်ရပါမယ်
- Connection အောင်မြင်မှသာ အခြား tabs များကို အသုံးပြုနိုင်ပါမယ်
