function caesarCipher(text: string, shift = 3) {
  return Array.from(text)
    .map((char) => {
      const code = char.charCodeAt(0);
      if (code < 65 || code > 90) return char;

      const shifted = ((code - 65 + shift) % 26) + 65;
      return String.fromCharCode(shifted);
    })
    .join("");
}

console.log(caesarCipher("CRYPTOGRAPHIE"));
