from pprint import pprint


power_dict = {}
with open("./power.csv", "r") as file:
    lines = file.readlines()
    for val in lines:
        pwm = int(val.split(",")[0].strip())
        val = float(val.split(",")[1].strip())
        for i in range(-2, 2):
            power_dict[pwm - i] = val

    file.close()


def calcnew(pwm_arr, maxAMP):
    ampcount = 0.0
    newarr = []
    for i in pwm_arr:
        ampcount += power_dict[i]
    print(ampcount)
    if ampcount > maxAMP:
        scaleFactor = maxAMP / ampcount
        newarr = [round(((pwm - 1500) * scaleFactor) + 1500) for pwm in pwm_arr]
    else:
        return pwm_arr
    ampcount = 0
    for i in newarr:
        ampcount += power_dict[i]
    print(ampcount)
    return newarr


if __name__ == "__main__":
    print(calcnew([1315, 1200, 1200, 1200], 10))
