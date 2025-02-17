function add(start=0){
    if(this.acc === undefined)
        this.acc = start;
    return this.acc++;
}

const f = new add(5)

console.log(f())