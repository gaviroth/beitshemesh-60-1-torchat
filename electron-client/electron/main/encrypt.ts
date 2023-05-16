import { MiddleManServer } from "./communicator";
import { spawn } from "child_process";
import path from "path";

const primes: number[] = [];
getPrimesInRange();
const [PRIVATE_KEY, PUBLIC_KEY, MY_NUM] = setKeys();

export async function encryptOnce(
  message: string,
  pk: string,
  n: string
): Promise<string> {
  const exePath = path.join(__dirname, "bin", "encrypt.exe");

  const encryptor = spawn(exePath, [message, pk, n]);
  let encrypted = "";
  for await (const data of encryptor.stdout) {
    encrypted += data;
  }
  return encrypted;
}

export async function decrypt(message: string): Promise<string> {
  const exePath = path.join(__dirname, "bin", "decrypt.exe");

  const decryptor = spawn(exePath, [
    message,
    PRIVATE_KEY.toString(),
    MY_NUM.toString(),
  ]);
  let decrypted = "";
  for await (const data of decryptor.stdout) {
    decrypted += data;
  }
  return decrypted;
}

export async function encrypt(
  data: string,
  route: MiddleManServer[]
): Promise<string> {
  let ret = data;

  return encryptOnce(data, route[0].publicKey, route[0].n).then((encrypted) => {
    return `${encrypted}${route[0].port}`;
  }).then((encrypted) => {
    return encryptOnce(encrypted, route[1].publicKey, route[1].n);
  }).then((encrypted) => {
    return `${encrypted}${route[1].port}`;
  }).then((encrypted) => {
    return encryptOnce(encrypted, route[2].publicKey, route[2].n);
  }).then((encrypted) => {
    return `${encrypted}${route[2].port}`;
  }).then((encrypted) => {
    return encryptOnce(encrypted, route[3].publicKey, route[3].n);
  }).then((encrypted) => {
    return `${encrypted}`;
  });
}

function GCD(num1: number, num2: number) {
  let Remainder: number;

  while (num2 != 0) {
    Remainder = num1 % num2;
    num1 = num2;
    num2 = Remainder;
  }

  return num1;
}

function getPrimesInRange() {
  for (let i = 2; i <= 250; i++) {
    let isPrime = true;

    // check if i is divisible by any number between 2 and i - 1
    for (let j = 2; j < i; j++) {
      if (i % j == 0) {
        isPrime = false;
        break;
      }
    }

    if (isPrime) {
      primes.push(i);
    }
  }
}

function pickRandomPrime(): number {
  const rand = Math.floor(Math.random() * primes.length);
  const num = primes[rand];
  primes.splice(rand, 1);
  return num;
}

function setKeys(): number[] {
  let my_num, my_private_key, my_public_key;
  let prime1 = pickRandomPrime(); // first prime number
  let prime2 = pickRandomPrime(); // second prime number
  // to check the prime numbers selected
  // cout<<prime1<<" "<<prime2<<endl;
  my_num = prime1 * prime2;
  let fi = (prime1 - 1) * (prime2 - 1);
  let e = 2;
  while (true) {
    if (GCD(e, fi) == 1) break;
    e++;
  } // d = (k*Φ(n) + 1) / e for some integer k
  my_public_key = e;
  let d = 2;
  while (true) {
    if ((d * e) % fi == 1) break;
    d++;
  }
  my_private_key = d;
  return [my_private_key, my_public_key, my_num];
}

/**
 * get the data for encryption
 * @returns [public_key, n]
 */
export function getPublicKeyAndN(): number[] {
  return [PUBLIC_KEY, MY_NUM];
}

export function getKeys() {
  return [PRIVATE_KEY, PUBLIC_KEY, MY_NUM];
}