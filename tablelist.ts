import Logger, { Level } from "./logger.js"
const logger = Logger.get("table-list")
const logEdit = Logger.get("editing")
const logData = Logger.get("data-svc")
const logLifeCycle = Logger.get("life-cycle")

/* *********************************** Typing **************************************** */
/**
 * Something like :
 * { title: 'Le Chardon et le Tartan', author: 'Diana GABALDON' }
 */
interface IColData {
    [key: string]: string
}
/**
 * {
 *  id: "0",
 *  columns : { title: 'Le Chardon et le Tartan', author: 'Diana GABALDON' }
 * }
 */
interface IRowData {
    id?: string,
    columns: IColData
}
/**
 * http data service should use this kind of data structure.
 * TODO : use this for the result of parsing json data.
 */
interface IServiceData {
    [key: string]: IColData
}
/**
 * A cell reference in this TableList for editing process.
 */
interface ICell {
    row: HTMLTableRowElement | null,
    col: HTMLTableCellElement | null
}
const NONE: ICell = { row: null, col: null }

enum KEY {
    NONE,
    TAB,
    SHIFT_TAB
}
enum BUTTON {
    NONE,
    ADD_1ST,
    ADD_ROW,
    DEL_ROW
}
/* ************************************** templating ******************************************** */
/**
 * Configuration data from component attributes.
 */
interface IConfig {
    /** button-add="fa fa-plus" */
    clsAdd: string,
    /** button-del="fa fa-times" */
    clsTimes: string,
    /** columns='{ "title" : "Titre", "author" : "Auteur"}' */
    keys: string[]     // [ "title", "author" ]
    headers: string[],  // [ "Titre", "Auteur" ]
}
/** A reducer lambda. */
const concat = (res: string, str: string) => res + str
/** 1st column button */
const tplButton = (cls: string) => `<i class="${cls}"></i>`
/** Header cell with column title */
const tplHeader = (ttlColumn: string) => `            <th>${ttlColumn}</th>
`
/** Table with header section and empty body */
const tplTable = (props: IConfig) => `<table class="w3-table-all">
    <thead>
        <tr>
            <th>${tplButton(props.clsAdd)}</th>
${props.headers.map((title: string) => tplHeader(title)).reduce(concat)}
        </tr>
    </thead>
    <tbody>
    </tbody>
</table>`
/** Optional id */
const tplId = (id?: string) => id == undefined ? "" : ` id="_${id}"`
/** Data cell with optional data */
const tplColumn = (data: string) => `    <td contenteditable="true">${data}</td>
`
/** Data row */
const tplLine = (props: IConfig, data: IRowData) => `<tr>
    <td${tplId(data.id)}>${tplButton(props.clsAdd)}&nbsp;&nbsp;${tplButton(props.clsTimes)}</td>
${props.keys.map(key => tplColumn(data.columns[key])).reduce(concat)}
</tr>`
/* *********************************** custom component **************************************** */
/**
 * A <table-list></table-list> web component for data service crud operations. Editing process
 * supports :
 * 
 * . tab/shift+tab navigation on the current row.
 * . automatic validation on Enter, tab on last column and click out.
 * . cancel editings on Escape key.
 * 
 * @see button-add, button-del, columns and data properties.
 */
export default class TableList extends HTMLElement {
    /** Cell being edited */
    protected cellEdited: ICell = NONE
    /** Restorable row data */
    protected cellsSaved: string[] = [] // TODO : replace by a IRowData ?
    /** validation allowance (true except on tab navigation) */
    protected tabbing: number = 0
    constructor() {
        super()
    }
    /* ********** Component life-cycle management methods ********** */
    /** Inserted in DOM */
    connectedCallback() {
        logLifeCycle.trace(`connectedCallback`)
        this.makeTable()
    }
    /**
     * Get name of component attributes to get "attribute changed" notifications.
     */
    static get observedAttributes() {
        return ["button-add", "button-del", "columns", "data"]
    }
    /**
     * Attribute changed notification.
     * 
     * @param namAttr Attribute name.
     * @param valNew New attribute value.
     * @param valOld Old attribute value.
     */
    attributeChangedCallback(namAttr: string, valNew: string, valOld: string) {
        logLifeCycle.trace(`attribute ${namAttr} changes from ${valOld} to ${valNew}`)
    }
    /**
     * Component removed from DOM notification.
     */
    disconncetedCallback() {
        logLifeCycle.trace(`disconncetedCallback`)
    }
    /** Component moved to a new DOM such as the one of a <iframe>. */
    adoptedCallback() {
        logLifeCycle.trace(`adoptedCallback`)
    }
    /* ********** Properties reflecting attributes ********** */
    /**
     * @return The "columns" attribute value.
     */
    get columns(): string {
        const attr = this.getAttribute("columns")
            ?? '{ "key0" : "Header0", "key1" : "Header1"}'
        return attr
    }
    /**
     * Set a new value to the "columns" attribute.
     * 
     * @param valNew Json string '{ "title" : "Titre", "author" : "Auteur"}' with
     * (Titre, Auteur) as columns display names and (title, author) as data column
     * names from data http service.
     * 
     * @see data property.
     */
    set columns(valNew: string) {
        this.setAttribute("columns", valNew)
    }
    /**
     * @return The "data" attribute value.
     */
    get data(): string {
        const attr = this.getAttribute("data") ?? ""
        return attr
    }
    /**
     * Set the base URL of the http service for data to edit.
     *
     * @param valNew A URL like "http://localhost:8000/book" supporting requests :
     * "get /book", "get /book/:id", "post /book", "put /book/:id" and "delete /book/:id".
     * The get /book provides a json string like "{
     *     '0': { title: 'Le Chardon et le Tartan', author: 'Diana GABALDON' },
     *     '1': { title: 'Germinal', author: 'Emile ZOLA' } }"
     * @see columns property for how to specify which data to show and in which order.
     */
    set data(valNew: string) {
        this.setAttribute("data", valNew)
    }
    /**
     * @return The "button-add" attribute value.
     */
    get buttonAdd(): string {
        const attr = this.getAttribute("button-add") ?? "fa fa-plus"
        return attr
    }
    /**
     * Set the CSS classes to define which icon to use for the "add row" button.
     * 
     * @param valNew CSS class list like "fa fa-plus".
     */
    set buttonAdd(valNew: string) {
        this.setAttribute("button-add", valNew)
    }
    /**
     * @return The "button-del" attribute value.
     */
    get buttonDel(): string {
        const attr = this.getAttribute("button-del") ?? "fa fa-times"
        return attr
    }
    /**
     * Set the CSS classes to define which icon to use for the "delete row" button.
     * 
     * @param valNew CSS class list like "fa fa-times".
     */
    set buttonDel(valNew: string) {
        this.setAttribute("button-del", valNew)
    }
    /**
     * @return Component configuration from attributes.
     */
    getProps(): IConfig {
        /* [columns] */
        const jsoColumns = JSON.parse(this.columns)
        return {
            clsAdd: this.buttonAdd,  // [button-add]
            clsTimes: this.buttonDel, // [button-del]
            headers: Object.values(jsoColumns),
            keys: Object.keys(jsoColumns)
        }
    }
    /**
     * Append a data row to table body.
     * 
     * @param tbody The table body.
     * @param data Row data.
     */
    appendRow(tbody: HTMLTableSectionElement, data: IRowData) {
        logEdit.debug("appendRow", data)
        const cell0 = tbody.querySelector("#_" + data.id)
        if (cell0 != null)
            return
        const props = this.getProps()
        const line = tplLine(props, data)
        tbody.insertAdjacentHTML("beforeend", line)
    }
    /**
     * Load data from http service to table rows ; only new data are
     * appended in new table rows. This requires the base URL of an
     * http data service to be set to the data attribute. 
     */
    loadData() {
        if (this.data == "")
            logData.warn("specify base URL of http data service in data attribute");
        logData.trace(`loadData from ${this.data}`);
        fetch(this.data, {
            method: "GET",
            headers: {
                "Accept": "application/json; charset=UTF-8"
            }
        }).then(resp => {
            return resp.json()
        }).then(map => {
            logData.trace(map);
            const table = this.querySelector("table")!
            const tbody = table.tBodies[0]
            for (let id in map) {
                this.appendRow(tbody, {
                    id: id,
                    columns: map[id]
                })
            }
        }).catch((reason: any) => {
            logData.trace(reason)
        })
    }
    /**
     * Insert table fully instrumented with data loaded from the
     * http service.
     */
    makeTable() {
        const table = this.querySelector("table")!
        if (table == null) {
            logLifeCycle.debug("insert table in table-list")
            const props = this.getProps()
            this.insertAdjacentHTML("afterbegin", tplTable(props))
            this.listenEvents()
            this.loadData()
        }
    }
    /**
     * Instrument the table for user editing interactions.
     */
    listenEvents() {
        logEdit.debug("listenEvents")
        let table = this.querySelector("table")!
        table.addEventListener("click", this.buttonListener.bind(this))
        const tbody = table.tBodies[0]
        tbody.addEventListener("focusin", this.focusInListener.bind(this))
        tbody.addEventListener("focusout", this.focusOutListener.bind(this))
        tbody.addEventListener("keydown", this.keyListener.bind(this))
    }
    /**
     * Make empty data row e.g. {"title" : "", "author" : ""}
     * from columns attribute e.g. { "title" : "Titre", "author" : "Auteur"}
     */
    makeEmptyDataRow(): IRowData {
        const columns = JSON.parse(this.columns)
        for (const key in columns)
            columns[key] = ""
        return { columns: columns }
    }
    /**
     * Make an empty table row from "columns" attribute
     */
    makeEmptyTableRow(): string {
        const dataRow = this.makeEmptyDataRow()
        const props = this.getProps()
        return tplLine(props, dataRow)
    }
    buttonKind(target: HTMLElement): number {
        if (target.tagName != "I")
            return BUTTON.NONE
        const tcell = target.parentElement! // th or td
        const tr = tcell.parentElement!
        if (target.className == this.buttonDel)
            return BUTTON.DEL_ROW
        else if (tcell.tagName == "TH")
            return BUTTON.ADD_1ST
        else
            return BUTTON.ADD_ROW
    }
    /**
     * The button click listener.
     * 
     * @param event The mouse event.
     */
    buttonListener(event: MouseEvent) {
        const target = <HTMLElement>event.target
        logEdit.debug("buttonListener", target.tagName)
        const tcell = target.parentElement! // th or td
        const tr = tcell.parentElement!
        const kind = this.buttonKind(target)
        if (kind == BUTTON.NONE)
            return
        const tableRow = this.makeEmptyTableRow()
        if (kind == BUTTON.ADD_1ST) {
            const table = target.closest("table")!
            const tbody = table.tBodies[0]
            tbody.insertAdjacentHTML("afterbegin", tableRow)
        } else if (kind == BUTTON.ADD_ROW)
            tr.insertAdjacentHTML("afterend", tableRow)
        else {// BUTTON.DEL_ROW
            // TODO : send DELETE request
            tr.remove()
        }
    }
    /* *********** Layered editing actions *********** */
    /**
     * Save table row data to string array.
     * 
     * @param row The table row.
     * @param cellsSaved The string array.
     */
    saveTo(row: HTMLTableRowElement, cellsSaved: string[]) {
        // TODO : why not save to IRowData ?
        logEdit.trace("saveTo");
        let len = -1
        Array.from(row.cells).forEach(cell => {
            if (len == -1) { // column 0 holds a id="_0" attribute
                len++
                return
            }
            cellsSaved[len++] = cell.innerText
        })
    }
    /**
     * Restore table row data from cellsSaved.
     * 
     * @see cellsSaved.
     */
    restore() {
        logEdit.trace("restore");
        let i = -1
        Array.from(this.cellEdited.row!.cells).forEach(cell => {
            if (i == -1) {
                i++
                return
            }
            cell.innerText = this.cellsSaved[i++]
        })
    }
    /**
     * Set table cell being edited to specified table cell.
     * 
     * @param cellNew New table cell to edit.
     */
    currentCell(cellNew: ICell) {
        logEdit.trace("currentCell");
        this.cellEdited = cellNew
    }
    /* ********** 2nd layer ********** */
    /**
     * Start row editing process with specified table cell.
     * 
     * @param cellStart Table cell/row to edit.
     */
    startEditing(cellStart: ICell) {
        logEdit.trace("startEditing");
        this.saveTo(cellStart.row!, this.cellsSaved)
        this.currentCell(cellStart)
    }
    /**
     * End current editing process and optionally start a new one.
     * 
     * @param cellStart Optional table cell/row to edit.
     */
    endEditing(cellStart?: ICell) {
        logEdit.trace("endEditing");
        const col = this.cellEdited.col!
        col.contentEditable = "false"
        col.contentEditable = "true"
        this.currentCell(NONE)
        if (cellStart) {
            this.startEditing(cellStart)
            return
        }
    }
    /**
     * Get row data from table row being edited.
     * TODO : merge with saveTo ?
     * 
     * @return Row data from table row.
     */
    dataFromRow(): IRowData {
        const row = this.cellEdited.row!
        const id = row.cells[0].id?.substring(1)
        const cells: string[] = []
        this.saveTo(row, cells)
        const columns = JSON.parse(this.columns)
        let i = 0
        for (const key in columns)
            columns[key] = cells[i++]
        return {
            id: id,
            columns: columns
        }
    }
    /**
     * Save data from row being edited to data service.
     */
    sendEditedRow() {
        logEdit.trace("sendEditedRow")
        const { id, columns } = this.dataFromRow()
        const url = this.data + (id ? `/${id}` : "")
        return fetch(url, {
            method: id ? "PUT" : "POST",
            body: JSON.stringify(columns),
            headers: {
                "Accept": "application/json; charset=UTF-8",
                "Content-type": "application/json; charset=UTF-8",
            }
        })
    }
    /* ********** Editing events management ********** */
    firstColumn(): boolean {
        return this.cellEdited.col == this.cellEdited.row!.children[1]
    }
    /**
     * @return true if cell being edited is the last of the row.
     */
    lastColumn(): boolean {
        return this.cellEdited.col == this.cellEdited.row!.lastElementChild
    }
    /**
     * Get cell/row from event target.
     * 
     * @param evt Event target.
     * @return cell/row.
     */
    getCell(evt: Event): ICell {
        const col = <HTMLTableCellElement>evt.target
        return <ICell>{
            col: col,
            row: <HTMLTableRowElement>col.parentElement
        }
    }
    /**
     * Safe validate the row editing process and optionally start a new one.
     * 
     * @param cellStart Optional new table cell/row to edit.
     */
    validate(cellStart?: ICell) {
        if (this.cellEdited == NONE)
            return
        this.sendEditedRow().then(resp => {
            this.endEditing(cellStart)
        }).catch((reason: any) => {
            this.restore()
            this.endEditing(cellStart)
        })
    }
    /* ********** Cell/row editing event listeners ********** */
    /**
     * Focus gained event listener.
     * 
     * @param FocusEvent Targeting cell gaining focus.
     */
    focusInListener(evt: FocusEvent) {
        logEdit.trace("focusInListener");
        const target = <HTMLElement>evt.target
        if (!target.isContentEditable) // TODO : ensure this code is useful
            return
        const cell = this.getCell(evt)
        if (this.cellEdited == NONE)
            this.startEditing(cell)
        else
            this.currentCell(cell)
    }
    /**
     * Focus lost event listener.
     * 
     * @param FocusEvent Targeting cell losing focus.
     */
    focusOutListener(evt: FocusEvent) {
        logEdit.trace("focusOutListener");
        const target = <HTMLElement>evt.target
        if (!target.isContentEditable)
            return
        switch (this.tabbing) {
            case KEY.TAB:
                if (this.lastColumn())
                    this.validate()
                break;
            case KEY.SHIFT_TAB:
                if (this.firstColumn()) {
                    this.restore()
                    this.endEditing()
                }
                break;
        }
        this.tabbing = KEY.NONE
    }
    /**
     * Key typed event listener.
     * 
     * @param evt Keyboard event.
     */
    keyListener(evt: KeyboardEvent) {
        logEdit.trace("keyListener");
        const target = <HTMLElement>evt.target
        if (!target.isContentEditable) // TODO : ensure this code is useful
            return
        logEdit.debug(evt);
        switch (evt.key) {
            case "Enter":
                evt.preventDefault() // don't enter a new line
                this.validate()
                break
            case "Tab":
                this.tabbing = evt.shiftKey ?
                    KEY.SHIFT_TAB : KEY.TAB
                break
            case "Escape":
                this.restore()
                this.endEditing()
                break
        }
    }
}
customElements.define("table-list", TableList)
/*
 * Roadmap :
 * . replace logger.trace by configurable logging
 * . process TODOs
 * . Externalize data service (easier to mock)
 * . Update table from service
 * . External data adapter function or class.
 */
